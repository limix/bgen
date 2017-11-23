from __future__ import unicode_literals

import os

folder = os.path.dirname(os.path.abspath(__file__))
with open(os.path.join(folder, '..', 'VERSION')) as f:
    version = f.read().strip()

templates_path = ['_templates']
source_suffix = '.rst'
master_doc = 'index'
project = 'bgen'
copyright = '2017, Danilo Horta'
author = 'Danilo Horta'
release = version
language = 'en'
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'conf.py']
pygments_style = 'sphinx'
todo_include_todos = False

import sphinx_rtd_theme
html_theme = "sphinx_rtd_theme"
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]

htmlhelp_basename = 'bgendoc'
