rm scalar.time vector.time
for _ in {1..100}; do
/usr/bin/time -f '%e' duckdb -unsigned <scalar.test 2>>scalar.time >/dev/null;
/usr/bin/time -f '%e' duckdb -unsigned <vector.test 2>>vector.time >/dev/null;
done
python3 <<EOF
with open('scalar.time','r') as f:
    st = [float(l) for l in f.readlines()]
with open('vector.time','r') as f:
    vt = [float(l) for l in f.readlines()]

print('scalar avg',sum(st)/len(st), 'min', min(st),'max',max(st))
print('vector avg',sum(vt)/len(vt),'min', min(vt), 'max', max(vt))
EOF
