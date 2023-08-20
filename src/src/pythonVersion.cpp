#include "venv_tool.hpp"

namespace venv_tool
{
    using namespace dataObject;
    using namespace fileSystem;
    using namespace logSystem;

    PythonVersion::PythonVersion(String version)
    {
        auto version_list = version.split(".");
        bool flag = true;
        if (len(version_list) == 3)
        {
            flag = false;
            for (int i = 0; i < 3; i++)
            {
                if (!version_list[i].isdigit())
                {
                    flag = true;
                    break;
                }
                switch (i)
                {
                case 0:
                    major = version_list[i].toInt();
                    break;
                case 1:
                    minor = version_list[i].toInt();
                    break;
                case 2:
                    patch = version_list[i].toInt();
                    break;
                }
            }
        }

        if (flag)
        {
            major = -1;
            minor = -1;
            patch = -1;
        }
    }

    int PythonVersion::getMajor() const { return major; }
    int PythonVersion::getMinor() const { return minor; }
    int PythonVersion::getPatch() const { return patch; }
    dataObject::String PythonVersion::getVersion() const
    {
        String ret = major;
        ret += ".";
        ret += minor;
        ret += ".";
        ret += patch;
        return ret;
    }

    bool PythonVersion::operator>(const PythonVersion &data) const
    {
        if (this->major < data.getMajor())
        {
            return false;
        }
        else if (this->major > data.getMajor())
        {
            return true;
        }

        if (this->minor < data.getMinor())
        {
            return false;
        }
        else if (this->minor > data.getMinor())
        {
            return true;
        }

        if (this->patch > data.getPatch())
        {
            return true;
        }
        return false;
    }

}