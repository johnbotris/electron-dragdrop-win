{
  "targets": [
    {
      "include_dirs" : [
        "<!(node -e \"require('nan')\")",
        "imagemagick/include/",
        "imagemagick/include/MagickCore/",
        "imagemagick/include/MagickWand/",
        "imagemagick/include/Magick++/"
      ],
      "target_name": "electron-dragdrop-win",
      "libraries": [ 
        "../imagemagick/lib/CORE_DB_bzlib_.lib",
        "../imagemagick/lib/CORE_DB_cairo_.lib",
        "../imagemagick/lib/CORE_DB_coders_.lib",
        "../imagemagick/lib/CORE_DB_croco_.lib",
        "../imagemagick/lib/CORE_DB_exr_.lib",
        "../imagemagick/lib/CORE_DB_ffi_.lib",
        "../imagemagick/lib/CORE_DB_filters_.lib",
        "../imagemagick/lib/CORE_DB_flif_.lib",
        "../imagemagick/lib/CORE_DB_glib_.lib",
        "../imagemagick/lib/CORE_DB_jp2_.lib",
        "../imagemagick/lib/CORE_DB_jpeg_.lib",
        "../imagemagick/lib/CORE_DB_lcms_.lib",
        "../imagemagick/lib/CORE_DB_libraw_.lib",
        "../imagemagick/lib/CORE_DB_librsvg_.lib",
        "../imagemagick/lib/CORE_DB_libxml_.lib",
        "../imagemagick/lib/CORE_DB_lqr_.lib",
        "../imagemagick/lib/CORE_DB_Magick++_.lib",
        "../imagemagick/lib/CORE_DB_MagickCore_.lib",
        "../imagemagick/lib/CORE_DB_MagickWand_.lib",
        "../imagemagick/lib/CORE_DB_openjpeg_.lib",
        "../imagemagick/lib/CORE_DB_pango_.lib",
        "../imagemagick/lib/CORE_DB_pixman_.lib",
        "../imagemagick/lib/CORE_DB_png_.lib",
        "../imagemagick/lib/CORE_DB_tiff_.lib",
        "../imagemagick/lib/CORE_DB_ttf_.lib",
        "../imagemagick/lib/CORE_DB_webp_.lib",
        "../imagemagick/lib/CORE_DB_zlib_.lib",
        "../imagemagick/lib/ImageMagickObject.lib" 
      ],
      "sources": [ 
        "addon.cc",
        "dataobject.cpp",
        "dropsource.cpp",
        "dropsourcenotify.cpp",
        "enumformat.cpp",
        "options.cpp",
        "clipboard_format.cpp"
      ]
    }
  ]
}