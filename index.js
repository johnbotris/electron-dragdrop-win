const dragDrop = require('bindings')('electron-dragdrop-win')

module.exports = {
    doDragDrop: dragDrop.DragDrop,
    whoAmI: dragDrop.WhoAmI
};
