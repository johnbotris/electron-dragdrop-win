{
  "targets": [
    {
      "include_dirs" : [
        "<!(node -e \"require('nan')\")",
      ],
      "target_name": "electron-dragdrop-win",
      "sources": [ 
        "addon.cc",
        "dataobject.cpp",
        "dropsource.cpp",
        "dropsourcenotify.cpp",
        "enumformat.cpp",
        "options.cpp"
      ]
    }
  ]
}