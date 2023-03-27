﻿#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include <winrt/Windows.Storage.Search.h>

using namespace winrt;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Search;
using namespace Windows::UI::Xaml;
using namespace winrt::Windows::ApplicationModel::Activation;

constexpr uint32_t BulkLoadStorageFileCount{ 50 };

namespace winrt::StorageQueryTest::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
    }


    void MainPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        BeginQueryWithActivatedFile();
    }

    IAsyncAction MainPage::ClickHandler_GetMoreStorageFiles(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
    {
        co_await ContinueQueryAsync();
    }


    IAsyncAction  MainPage::ClickHandler_GetFileCount(IInspectable const&, RoutedEventArgs const&)
    {
        m_queryTotalItemCount = co_await m_query.GetItemCountAsync();
        debugPrint(L"GetItemCountAsync() %d files\n", m_queryTotalItemCount);
    }

    void MainPage::OnFileSystemChangeNotification(WS::Search::IStorageQueryResultBase /*sender*/, Windows::Foundation::IInspectable /*args*/)
    {
        debugPrint(L"OnFileSystemChangeNotification()\n")
    }

    winrt::fire_and_forget MainPage::BeginQueryWithActivatedFile()
    {
        auto strong_this{ get_strong() };

        m_query = KnownFolders::PicturesLibrary().CreateFileQuery();
        m_fileSystemWatchToken = m_query.ContentsChanged({ get_strong(), &MainPage::OnFileSystemChangeNotification });
        m_queryTotalItemCount = co_await m_query.GetItemCountAsync();
        debugPrint(L"GetItemCountAsync() %d files\n", m_queryTotalItemCount);
        m_queryIndex = 0;
        auto filesInQueryResult = co_await m_query.GetFilesAsync(m_queryIndex, 16);
        debugPrint(L"GetFilesAsync(%d, %d) returned %d files\n", m_queryIndex, 16, filesInQueryResult.Size());
        m_queryIndex += filesInQueryResult.Size();
    }

    IAsyncAction MainPage::ContinueQueryAsync()
    {
        auto strong_this{ get_strong() };

        if (m_query != nullptr)
        {
            auto filesInQueryResult = co_await m_query.GetFilesAsync(m_queryIndex, BulkLoadStorageFileCount);
            auto resultCount = filesInQueryResult.Size();
            debugPrint(L"GetFilesAsync(%d, %d) returned %d files\n", m_queryIndex, BulkLoadStorageFileCount, resultCount);
            m_queryIndex += resultCount;

            if (resultCount > 0)
            {
                const auto firstFile = filesInQueryResult.GetAt(0);
                const auto firstFileName = firstFile.Name();
                debugPrint(L"first file in results: %ls ", firstFileName.c_str());
                if (resultCount > 1)
                {
                    const auto lastFile = filesInQueryResult.GetAt(resultCount - 1);
                    const auto lastFileName = lastFile.Name();
                    debugPrint(L", last file in results: %ls ", lastFileName.c_str());
                }
                debugPrint(L"\n");
            }

            if (filesInQueryResult.Size() == 0 && m_queryIndex < m_queryTotalItemCount)
            {
                debugPrint(L"no more items, Something wrong\n");
                co_return;
            }
        }
    }

    IAsyncAction MainPage::GetFileCountAsync()
    {
        auto strong_this{ get_strong() };
        auto totalItemCount = co_await m_query.GetItemCountAsync();
        debugPrint(L"GetItemCountAsync() %d files\n", totalItemCount);
    }

}
