# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import subprocess
subprocess.call('doxygen', shell=True)
html_extra_path = ['build/html']
html_extra_path = ['build/xml']

project = 'MAPLE'
copyright = '2024, Corey Knutson'
author = 'Corey Knutson'
release = '0.1.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
# add custom css for tables, etc.
html_css_files = [
    "css/tables.css"
]

extensions = ["breathe", "sphinx.ext.autosectionlabel", "sphinx_tabs.tabs"]
breathe_projects = {
    'MAPLE': './xml'
}

# Breathe configuration
breathe_default_project = "MAPLE"

# Tabs for code, prevent tab from closing
sphinx_tabs_disable_tab_closing = True