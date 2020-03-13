from __future__ import unicode_literals

from datetime import datetime
import os
import subprocess
import sys
import re

import sphinx_rtd_theme

_folder = os.path.dirname(os.path.abspath(__file__))
with open(os.path.join(_folder, "..", "CMakeLists.txt")) as f:
    s = re.search(r"bgen VERSION (\d+\.\d+\.\d+)", f.read())
    if s is None:
        raise RuntimeError("Could not fetch version")
    version = s.groups()[0]

extensions = ["sphinx.ext.viewcode", "breathe"]
breathe_projects = {"bgen": "doxyxml/"}
breathe_default_project = "bgen"
templates_path = ["_templates"]
source_suffix = ".rst"
master_doc = "index"
project = "bgen"
copyright = f"2017-{datetime.now().year}, Danilo Horta"
author = "Danilo Horta"
release = version
language = "en"
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store", "conf.py"]
todo_include_todos = False
primary_domain = "c"

pygments_style = "default"
html_theme = "sphinx_rtd_theme"
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]

htmlhelp_basename = "bgendoc"


def run_doxygen(folder):
    """Run the doxygen make command in the designated folder"""

    try:
        retcode = subprocess.call("cd %s; make doxygen" % folder, shell=True)
        if retcode < 0:
            sys.stderr.write("doxygen terminated by signal %s" % (-retcode))
    except OSError as e:
        sys.stderr.write("doxygen execution failed: %s" % e)


def generate_doxygen_xml(*_):
    """Run the doxygen make commands if we're on the ReadTheDocs server"""

    read_the_docs_build = os.environ.get("READTHEDOCS", None) == "True"

    if read_the_docs_build:
        run_doxygen(".")


def setup(app):
    # Add hook for building doxygen xml when needed
    app.connect("builder-inited", generate_doxygen_xml)
