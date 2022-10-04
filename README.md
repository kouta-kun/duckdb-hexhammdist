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

Building the test task compiles the whole DuckDB tree, while make release/all/debug only compile as much as required for an extension.

## Usage
```bash
$ duckdb -unsigned

INSTALL 'build/release/extension/duckdb-hexhammdist/hexhammdist_extension.duckdb_extension';
LOAD 'hexhammdist_extension';
select hexhammdist('abc','def');
```