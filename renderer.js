// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

const ipc = require('electron').ipcRenderer

const syncMsgBtn = document.getElementById('sync-msg')

// syncMsgBtn.addEventListener('mousedown', function () {
//   const reply = ipc.sendSync('synchronous-message', 'ping')
//   const message = `Synchronous message reply: ${reply}`
//   document.getElementById('sync-reply').innerHTML = message
// })
