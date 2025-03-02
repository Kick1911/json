import json

arr = []

for i in range(10000):
    arr.append({"id": i, "line_data": i * 2})

res = str(json.dumps(arr))
