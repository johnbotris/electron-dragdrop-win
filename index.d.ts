/**
 * @param dataPromise A Promise of a list of file paths. If the list is empty or the promise rejects, no drop will occur
 */
// declare export function doDragDrop(dataPromise: Promise<Array<string>>): Promise<void>;
 declare export function doDragDrop(files: Array<FileInfo>): Promise<void>;

interface FileInfo {
    name: string,
    data: Uint8Array
}