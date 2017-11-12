#ifndef _pen_json_h
#define _pen_json_h

#include "definitions.h"
#include "jsmn/jsmn.h"
#include "str/Str.h"

namespace pen
{
    struct json_object;
    
    class json
    {
    public:
        ~json();
        json( );
        json( const json& other );
        
        static json load_from_file( const c8* filename );
        static json load( const c8* json_str );
        static json combine( const json& j1, const json& j2, s32 indent = 0 );
        
        Str         dumps();
        
        Str         name();
        jsmntype_t  type();
        
        u32         size() const;
        
        json operator [] (const c8* name) const;
        json operator [] (const u32 index) const;
        json& operator = (const json& other);
        
        Str     as_str();
        u32     as_u32();
        s32     as_s32();
        bool    as_bool();
        f32     as_f32();
                
    private:
        json_object* m_internal_object;
        void copy( json* dst, const json& other );
    };
}

#endif

