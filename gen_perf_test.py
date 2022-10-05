import random
from tqdm import tqdm

hexTab = [hex(i)[2:] for i in range(16)]

rds = []

for i in tqdm(range(2**20)):
    ln = random.randint(8,1024)
    a = ''.join(hexTab[random.randint(0,15)] for _ in range(ln))
    b = ''.join(hexTab[random.randint(0,15)] for _ in range(ln))
    rds.append(','.join([a,b]))

with open('tests.csv','w') as f:
    f.write('value_a,value_b\n')
    for r in rds:
        f.write(r+'\n')

