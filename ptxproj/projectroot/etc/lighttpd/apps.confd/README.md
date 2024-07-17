# Settings for web applications

This directory contains settings for specific web applications, 
such as WBM, CODESYS WebVisu or others.

Each file with the `.conf` extension is loaded automatically on lighttpd
startup. Additional files inside subfolders are **not** loaded automatically.
This allows to store application specific configuration options inside specific
folders for each application. Therefore you may find folders like `wbm` or 
`webvisu` to exist here beside their respective `.conf` files.
