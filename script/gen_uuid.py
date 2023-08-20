import uuid

"""
类似于gen_guid.exe,随机生成一个UUID
"""
print()
result = uuid.uuid1()
print(repr(result))

text = str(result).upper()
print(f'"{text}"')

# 格式化成结构体格式
print("{", end='')
print(f"0x{text[0:8]}, ", end='')

print(f"0x{text[9:13]}, ", end='')
print(f"0x{text[14:18]}, ", end='')

print("{", end='')
print(f"0x{text[19:21]}, ", end='')
print(f"0x{text[21:23]}, ", end='')

print(f"0x{text[24:26]}, ", end='')
print(f"0x{text[26:28]}, ", end='')
print(f"0x{text[28:30]}, ", end='')
print(f"0x{text[30:32]}, ", end='')
print(f"0x{text[32:34]}, ", end='')
print(f"0x{text[34:36]}", end='')
print("}", end='')

print("}")
print()