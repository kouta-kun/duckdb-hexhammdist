# DuckDB hexadecimal hamming distance extension

This extension implements the hexhammdist for DuckDB, ported from https://github.com/droe/sqlite-hexhammdist

## How to
```bash
git clone https://github.com/kouta-kun/duckdb-hexhammdist
cd duckdb-hexhammdist
make
# for testing
make test
build/release/test/unittest --test-dir . "[hexhamm]"
```

Building the test task compiles the whole DuckDB tree, while make release/all/debug only compile as much as required for an extension. Additionally, `make ext-only` builds the extension in shared library mode. This can generate issues when used with a distribution/official release, so it is not recommended.

The `make perf-test` task compares performance of the scalar and vectorized versions, currently the vectorized version barely matches the average performance of the scalar version and ocasionally has slightly better worst-case performance. According to the DuckDB devs in the Discord, both scalar and vectorized functions are automatically parallelized by DuckDB so it's possible that they've both reached peak performance through it.

Currently building on Ubuntu 20.04 seems to generate a broken extension that does not load on v0.5.1 (produces segfault). Docker instructions seem to work correctly but at a far higher RAM consumption:

```bash
docker build -t hexhamm-build .
docker create --name hexhamm hexhamm-build
docker cp hexhamm:/duckdb-hexhammdist/build/release/extension/duckdb-hexhammdist/hexhammdist_extension.duckdb_extension .
```

## Usage
```bash
$ duckdb -unsigned

INSTALL 'build/release/extension/duckdb-hexhammdist/hexhammdist_extension.duckdb_extension';
LOAD 'hexhammdist_extension';
select hexhammdist('abc','def');
```
