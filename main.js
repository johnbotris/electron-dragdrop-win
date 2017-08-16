const electron = require('electron')


// Module to control application life.
const app = electron.app
// Module to create native browser window.
const BrowserWindow = electron.BrowserWindow

const fs = require('fs');
const path = require('path')
const url = require('url')
const ipc = require('electron').ipcMain
const dragDrop = require('./index');

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow

function createWindow () {
  // Create the browser window.
  mainWindow = new BrowserWindow({width: 800, height: 600})

  // and load the index.html of the app.
  mainWindow.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
  }))

  // Open the DevTools.
  mainWindow.webContents.openDevTools()

  // Emitted when the window is closed.
  mainWindow.on('closed', function () {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null
  })
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow)

// Quit when all windows are closed.
app.on('window-all-closed', function () {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

app.on('activate', function () {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    createWindow()
  }
})

ipc.on('synchronous-message', function (event, arg) {
  // var cfTextFormat = dragDrop.createStandardAsciiTextFormat("Ashkan Daie");
  // var cfUnicodeTextFormat = dragDrop.createStandardUnicodeTextFormat("Ashkan Daie");
  // var filename = path.join(__dirname, 'obama.png');
  // var imageBuffer = fs.readFileSync(filename);
  // var cfBitmapFormat = dragDrop.createStandardBitmapFormat(imageBuffer);
  // console.log(cfBitmapFormat.format, cfTextFormat.data.length);

  var filename = path.join(__dirname, 'options.cpp');
  var data = "<xml>Sample Drag and Drop Payload</xml>";
  var ascii = new Buffer(data, 'ascii');
  var utf8 = new Buffer(data, 'utf8');
  var unicode = new Buffer(data, 'utf16le');
  
  var options = {
    windowHandle: mainWindow.getNativeWindowHandle(),
    formats: {
      CF_ASCII: ascii,
      CF_UTF8: utf8,
      CF_UNICODE: unicode
    },
    events: {
      completed: (data) => console.log('complete', data),
      dragEnter: (data) => console.log('dragEnter', data),
      dragLeave: () => console.log('dragLeave')
    }
  };

  console.log(options);

  dragDrop.doDragDrop(options);
  event.returnValue = "ok";
  // event.returnValue = 'pong'
})

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.