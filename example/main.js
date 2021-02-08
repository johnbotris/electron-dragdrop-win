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
        console.log("dragging start")
        // const file = "./1280px-Slovenia_location_map.svg.png"
        // const icon = nativeImage.createFromPath("./icon.png")
        // var cfTextFormat = dragDrop.createStandardAsciiTextFormat("Ashkan Daie");
        // var cfUnicodeTextFormat = dragDrop.createStandardUnicodeTextFormat("Ashkan Daie");
        // var filename = path.join(__dirname, 'obama.png');
        // var imageBuffer = fs.readFileSync(filename);
        // var cfBitmapFormat = dragDrop.createStandardBitmapFormat(imageBuffer);
        // console.log(cfBitmapFormat.format, cfTextFormat.data.length);

        // const fetched = fetch("http://192.168.178.35:9000/iconsmaller.png")
        //     .then(response => {
        //         console.log("response", response)
        //     })
        //     .catch(err => console.log("err", err))

        return dragDrop.doDragDrop(Promise.resolve(["C:\\Users\\User\\text.txt"]));
    })
}

app.whenReady().then(startup)
app.on('activate', () => !win && startup())

app.on('window-all-closed', () => {
    app.quit()
})


