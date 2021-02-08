{
  'targets': [
    {
      'target_name': 'electron-dragdrop-win',
      'include_dirs': [
        '<!(node -e "require(\'nan\')")',
      ],
      'defines': [ 'UNICODE', '_UNICODE', 'WIN32_LEAN_AND_MEAN' ],
      'sources': [
      ],
      'conditions': [
        ['OS=="win"', {
          'sources': [
            "src/addon.cpp",
            "src/Worker.cpp",
            "src/ole/DataObject.cpp",
            "src/ole/DropSource.cpp",
            "src/ole/oleutils.cpp",
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
