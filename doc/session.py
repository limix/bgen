# coding: utf-8
import sphinx
sphinx.ext
import sphinx.ext
import sphinx.ext.napoleon
sphinx.ext.napoleon.NumpyDocstring
get_ipython().magic('cd doc')
get_ipython().magic('ls ')
f = open("api.rst", "r")
txt = f.read()
f.close()
txt
n = sphinx.ext.napoleon.NumpyDocstring(txt)
n.lines
n.lines[0]
get_ipython().magic('pinfo n.lines')
lines = n.lines()
lines[0]
lines[1]
lines[2]
lines[3]
lines[4]
lines[5]
lines[6]
lines[7]
lines[8]
lines[9]
lines[10]
lines[11]
lines[12]
lines[13]
lines[14]
