# -*- coding: UTF-8 -*-

print(r"""#1\\""")
print(r"""#2\"""")
print(r"""#3\"""a;#x""")
print(r'''#4\'''b;#x''')
print(r"#5\"c;#x")
print(r'#6\'d;#x')

'''
multi comment

"""
print(R"""Hello Python")
"""
'''

print("#1\\") # ok
print("#2\"")
print("#3\"a")

print('''
# not comment

"""
not comment
"""
''')

"""
multi comment
'''
# 
'''

print("Hello Python")

"""

print("""
# not comment

'''
not comment
'''

""")
