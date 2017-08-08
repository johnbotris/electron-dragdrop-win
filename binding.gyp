{
  "targets": [
    {
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
      "target_name": "electron-dragdrop-win",
      "sources": [ 
        "addon.cc",
        "dataobject.cc",
        "dropsource.cc",
        "enumformat.cc"
      ]
    }
  ]
}