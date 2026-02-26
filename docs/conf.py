import os
from textwrap import dedent

version = os.environ.get("BUILDVER", default="latest")
project = 'Astra'
copyright = '2026 Owen Z. Siebers'
author = 'Owen Z. Siebers (RobotLeopard86)'
release = version

extensions = [
	'breathe',
	'exhale',
	'myst_parser'
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'README.md', 'LICENSE.md', '.venv']

html_theme = 'pydata_sphinx_theme'
html_title = "Astra Documentation"
html_favicon = "../astra_logo.png"
html_permalinks_icon = "<span/>"
html_use_index = False
html_domain_indices = False
html_copy_source = False
html_static_path = ["assets"]
html_css_files = ["fonts.css"]

breathe_projects = {
    "Astra API Reference": "./.doxygen/xml"
}
breathe_default_project = "Astra API Reference"

exhale_args = {
    "containmentFolder":     "./api",
    "rootFileName":          "index.rst",
    "doxygenStripFromPath":  "../",
    "rootFileTitle":         "Astra API Reference",
    "createTreeView":        True,
    "exhaleExecutesDoxygen": True,
    "afterTitleDescription": "Welcome to the Astra API documentation. Here, you can find detailed information on Astra's public interface.",
    "exhaleDoxygenStdin": dedent('''
									INPUT = ../include
                                    EXCLUDE_SYMBOLS = std*,LJAPI,LJBUILD
									HIDE_UNDOC_MEMBERS = YES
									MAX_INITIALIZER_LINES = 0
								 	INPUT_ENCODING = UTF-8
									''')
}

primary_domain = 'cpp'
highlight_language = 'cpp'

html_context = {
   "default_mode": "dark"
}

html_theme_options = {
    "logo": {
        "text": "Astra Documentation",
        "image_light": html_favicon,
        "image_dark": html_favicon
    },
    "icon_links": [
        {
            "name": "GitHub",
            "url": "https://github.com/RobotLeopard86/Astra",
            "icon": "fa-brands fa-github",
            "type": "fontawesome",
        }
   ],
   "navbar_end": ["version-switcher", "theme-switcher", "navbar-icon-links",],
   "switcher": {
        "version_match": version,
        "json_url": "https://raw.githubusercontent.com/RobotLeopard86/Astra/refs/heads/main/docs/switcher.json"
    },
    "announcement": "Astra is undergoing heavy updates at the moment. Do not expect this documentation to remain stable."
}
