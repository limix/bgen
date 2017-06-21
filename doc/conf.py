#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os

import sphinx_rtd_theme

folder = os.path.dirname(os.path.abspath(__file__))
with open(os.path.join(folder, '../VERSION')) as f:
    version = f.read().strip()

extensions = ['sphinx.ext.mathjax']
templates_path = ['_templates']
source_suffix = '.rst'
master_doc = 'index'
project = 'bgen-reader'
copyright = '2017, Danilo Horta'
author = 'Danilo Horta'

release = version

language = 'c'

exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'conf.py']

pygments_style = 'sphinx'

todo_include_todos = False

html_theme = 'sphinx_rtd_theme'
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]
htmlhelp_basename = 'bgen-readerdoc'

latex_elements = {}

latex_documents = [
    (master_doc, 'bgen-reader.tex', 'bgen-reader Documentation',
     'Danilo Horta', 'manual'),
]

man_pages = [(master_doc, 'bgen-reader', 'bgen-reader Documentation', [author],
              1)]

texinfo_documents = [
    (master_doc, 'bgen-reader', 'bgen-reader Documentation', author,
     'bgen-reader', 'One line description of project.', 'Miscellaneous'),
]
