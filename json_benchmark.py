import json

with open("kpd.db") as f:
    a = json.loads(f.read())
    print(json.dumps(a))
