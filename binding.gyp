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
            "src/addon.cpp",
            "src/dataobject.cpp",
            "src/dropsource.cpp",
            "src/options.cpp",
            "src/utils.cpp"
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
