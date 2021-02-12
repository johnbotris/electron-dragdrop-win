const { app, BrowserWindow, ipcMain } = require('electron')
const path = require('path')
const dragDrop = require('../index');

function startup() {
    const win = new BrowserWindow({
        width: 400,
        height: 400,
        webPreferences: {
            nodeIntegration: true,
        }
    })
    win.loadFile('./index.html')
    win.webContents.openDevTools()



    ipcMain.handle('drag', (event, ...args) => {
        return dragDrop.doDragDrop([
            {
                name: "test.txt",
                data: Buffer.from("this test text isn't so long, is it?"),
            },
            {
                name: "story.txt",
                data: Buffer.from("it was a cold, dark, stormy, night"),
            },
        ]);
    })
}

app.whenReady().then(startup)
app.on('activate', () => !win && startup())

app.on('window-all-closed', () => {
    app.quit()
})


