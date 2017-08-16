{
  'targets': [
    {
      'target_name': 'electron-dragdrop-win',
      'include_dirs': [
        '<!(node -e "require(\'nan\')")'
      ],
      'sources': [
      ],
      'conditions': [
        ['OS=="win"', {
          'sources': [
            "addon.cc",
            "dataobject.cpp",
            "dropsource.cpp",
            "dropsourcenotify.cpp",
            "enumformat.cpp",
            "options.cpp"
          ],
        }],
        ['OS!="win"', {
          'sources': [
            "addon-unsupported-platform.cc"
          ],
        }]
      ]
    }
  ]
}
