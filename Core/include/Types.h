//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_TYPES_H
#define EDGECAFFE_TYPES_H

namespace EdgeCaffe
{
    class Type
    {
    public:
        enum MODE_TYPE {
            BULK = 0,
            DEEPEYE = 1,
            MASA = 2,
            PARTIAL = 3,
            LINEAR = 4,
            EXECPRIO= 5,
            EXECPRIO_INTER= 6
        };
        inline const static std::map<std::string, MODE_TYPE> mapToVals = {
                {"bulk", BULK},
                {"deepeye",  DEEPEYE},
                {"partial",  PARTIAL},
                {"linear",  LINEAR},
                {"masa", MASA},
                {"execprio", EXECPRIO},
                {"execprio-inter", EXECPRIO_INTER}

        };

        inline const static std::unordered_map<std::string, int> memory_values = {
                  {"4G", 4096}
                , {"2G", 2048}
                , {"1G", 1024}
                , {"512M", 512}
                , {"256M", 256}
                , {"128M", 128}

        };
    };
}

#endif //EDGECAFFE_TYPES_H
