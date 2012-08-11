#ifndef __LIBED2K_TRANSFER_HANDLE__
#define __LIBED2K_TRANSFER_HANDLE__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <libtorrent/peer_info.hpp>
#include "libed2k/md4_hash.hpp"

namespace libed2k
{
    class transfer;
    namespace aux
    {
        class session_impl_base;
        class session_impl;
        class session_impl_test;
        class session_fast_rd;
    }

    // We will usually have to store our transfer handles somewhere, 
    // since it's the object through which we retrieve information 
    // about the transfer and aborts the transfer.
    struct transfer_handle
    {
        friend class aux::session_impl_base;
        friend class aux::session_impl;
        friend class aux::session_impl_test;
        friend class aux::session_fast_rd;
        friend class transfer;

        transfer_handle() {}

        bool is_valid() const;

        md4_hash hash() const;
        fs::path filepath() const;
        fsize_t filesize() const;
        bool is_seed() const;
        bool is_finished() const;
        bool is_paused() const;
        bool is_aborted() const;
        bool is_announced() const;
        transfer_status status() const;
        void get_peer_info(std::vector<peer_info>& infos) const;

        void piece_availability(std::vector<int>& avail) const;
        void set_piece_priority(int index, int priority) const;
        int piece_priority(int index) const;
        bool is_sequential_download() const;
        void set_sequential_download(bool sd) const;
        void set_upload_limit(int limit) const;
        int upload_limit() const;
        void set_download_limit(int limit) const;
        int download_limit() const;

        void pause() const;
        void resume() const;

        size_t num_pieces() const;
        int num_peers() const;
        int num_seeds() const;
        fs::path save_path() const;
        void save_resume_data() const;

        void move_storage(fs::path const& save_path) const;
        bool rename_file(const std::string& name) const;

        bool operator==(const transfer_handle& h) const
        { return m_transfer.lock() == h.m_transfer.lock(); }

        bool operator!=(const transfer_handle& h) const
        { return m_transfer.lock() != h.m_transfer.lock(); }

        bool operator<(const transfer_handle& h) const
        { return m_transfer.lock() < h.m_transfer.lock(); }
    private:

        transfer_handle(const boost::weak_ptr<transfer>& t):
            m_transfer(t)
        {}

        boost::weak_ptr<transfer> m_transfer;
    };

    extern std::string transfer_status2string(const transfer_status& s);
}

#endif
