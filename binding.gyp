{
  'targets': [
    {
      'target_name': 'electron-dragdrop-win',
      'include_dirs': [
        '<!(node -e "require(\'nan\')")',
      ],
      'defines': [ 'UNICODE', '_UNICODE'],
      'sources': [
      ],
      'conditions': [
        ['OS=="win"', {
          'sources': [
            "src/addon.cpp",
            "src/Worker.cpp",
            "src/v8utils.cpp",
            "src/ole/DataObject.cpp",
            "src/ole/DropSource.cpp",
            "src/ole/EnumFormat.cpp",
            "src/ole/Stream.cpp",
            "src/ole/ole.cpp"
          ],
        }],
        ['OS!="win"', {
          'sources': [
            "src/addon-unsupported-platform.cc"
          ],
        }]
      ]
    }
  ]
}
