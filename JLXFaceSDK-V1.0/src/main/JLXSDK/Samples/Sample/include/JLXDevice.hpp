#ifndef __JLX_DEVICE_HPP
#define __JLX_DEVICE_HPP
#include "Samples.hpp"

namespace JLX{
    namespace GTI{
        class Device{
        public:
        private:
          std::string   m_deviceName;
          std::string   m_coefName;
          int           feat_vector_len;

        private:
          GtiDevice   *m_Device;
          TCHAR       *m_FileNameBuffer;
          float       *m_Buffer32FC3;
          BYTE        *m_Img224x224x3Buffer;
          float       *m_ImageOutPtr;
          int         m_WtRdDelay = 35;        // Read/Write delay
          int         m_GnetType;

          ULONGLONG m_SvicTimerStart = 0;
          ULONGLONG m_SvicTimerEnd = 0;
          ULONGLONG m_CnnTimerStart = 0;
          ULONGLONG m_CnnTimerEnd = 0;
          ULONGLONG m_FcTimerStart = 0;
          ULONGLONG m_FcTimerEnd = 0;
          ULONGLONG m_PureCnnStart = 0;
          ULONGLONG m_PureCnnEnd = 0;
          ULONGLONG m_IdentifyTimerStart = 0;
          ULONGLONG m_IdentifyTimerEnd = 0;
        };
    };
};


#endif
