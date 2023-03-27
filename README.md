# Windows.Storage.Search.StorageFileQueryResult Faulty State after Idle
------
This sample app is used to demonstrate a problem we are encountering where StorageFileQueryResult enters a faulty state after idle:

Repro steps:

Setup:
1. Have folder with 1000+ files.
Here's a powershell script to generate 10k empty text files in the current directory.
`1..10000 | %{ "file$_.txt" | Out-File $_ -Force }`
2. Add this folder to the Pictures Library:
Right click File Explorer left-navigation bar on top of 'This PC', check show libraries', drag and drop the large folder to the Pictures library.
3. Clone this repo.

Bug repro steps:
5. Build launch the app with the Debug/x64 config and a debugger attached. 
6. Click the app's 'Load More StorageFiles' button until about 1000 items are queried - this can be checked in the debug console.
7. Wait for about 1-2 minutes.
8. Click 'Load More StorageFiles' again, and StorageFileQueryResult.GetFilesAsync fails to return any files. The 'Get File Count' button will also hang indefinitely in StorageFileQueryResult.GetFilesAsync.
9. It may take a couple repros to get StorageFileQueryResult into this state and after a period in this faulty state, it may start working again.
