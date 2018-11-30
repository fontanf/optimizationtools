#pragma once

#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#define STR1(x) std::string(   ) + #x + " " + std::to_string(x)
#define STR2(x) std::string(" ") + #x + " " + std::to_string(x)
#define STR3(x, y) std::string(   ) + #x + " " + std::to_string(y)
#define STR4(x, y) std::string(" ") + #x + " " + std::to_string(y)

#ifdef NDEBUG
#define DBG(x)
#else
#define DBG(x) x
#endif

struct Info
{
    Info(): t1(std::chrono::high_resolution_clock::now()) { }

    boost::property_tree::ptree pt;
    bool verbose_  = false;
    bool dbg_      = false;
    bool dbg_live_ = false;
    std::string debug_string_;
    std::string ini_file_ = "";
    std::string debug_file_ = "";
    std::chrono::high_resolution_clock::time_point t1;

    /**
     * Setters
     */

    void set_verbose()         { verbose_  = true; }
    void set_verbose(bool b)   { verbose_  = b; }

    void set_debug()           { dbg_      = true; }
    void set_debug(bool b)     { dbg_      = b; }

    void set_debuglive()       { dbg_live_ = true; }
    void set_debuglive(bool b) { dbg_live_ = b; }

    void set_inifile(std::string f) { ini_file_ = f; }
    void set_debugfile(std::string f) { debug_file_ = f; }

    /**
     * Getters
     */

    bool verbose() const { return verbose_; }
    bool debug()   const { return (dbg_ || dbg_live_); }

    double elapsed_time() const
    {
        std::chrono::high_resolution_clock::time_point t2
            = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        return time_span.count();
    }

    /**
     * Modifiers
     */

    void verbose(std::string message)
    {
        if (verbose_ || dbg_live_)
            std::cout << message;
        if (dbg_)
            debug_string_ += message;
    }

    void debug(std::string message)
    {
        if (dbg_)
            debug_string_ += message;
        if (dbg_live_)
            std::cout << message;
    }

    /**
     * Writers
     */

    void write_ini() const { write_ini(ini_file_); }
    void write_ini(std::string file) const
    {
        if (file != "")
            boost::property_tree::write_ini(file, pt);
    }

    std::string debug_string() const { return debug_string_; }

    void write_dbg() const { write_dbg(debug_file_); }
    void write_dbg(std::string file) const
    {
        if (file != "") {
            std::ofstream f(file);
            f << debug_string_;
            f.close();
        }
    }

    /**
     * Others
     */

    template <class T>
    static std::string to_string(T t)
    {
        std::ostringstream s;
        s << t;
        return s.str();
    }

};

