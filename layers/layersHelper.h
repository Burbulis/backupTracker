#include <string>
#include "layersHelper.hpp"

#define SET_LAYERS_DBG
#define CREATE_NEW_LAYER_DBG
#define REFRESH_DBG

#ifndef __LAYER_HELPER_CLASS__
#define __LAYER_HELPER_CLASS__
namespace Layers
{

    class layersHelper:protected LayersIO::layersHelper
    {
       	std::tm  strToTime(std::string time_s);
        public:

        layersHelper();
        layersHelper(const std::string& _fileGuid);
        std::string  getFileGuid(void) const;

        void setLayers(void);
        void setLayers(std::string fileGuid);	
        void resetLayers(void);

        bool Empty(void);

        std::string  createNewLayer(void);

        SQLCMD::types::ul_long getFirstLayerId(void);
        SQLCMD::types::ul_long getLastLayerId(void);


        std::optional <SQLCMD::types::ul_long>  getIdByHash(SQLCMD::types::ul_long layerHash);
        std::optional <SQLCMD::types::ul_long>  getIdByGuid(std::string  guid);
        std::optional <SQLCMD::types::ul_long>  getHashById(SQLCMD::types::ul_long layerId);
        std::optional <std::string>             getGuidById(SQLCMD::types::ul_long layerId);
        std::optional <std::tm>                 getTimeById(SQLCMD::types::ul_long layerId);
        
        std::vector<std::string> getDateTimes(void);
        std::vector<SQLCMD::types::ul_long> getHashes(void);
        std::vector<SQLCMD::types::ul_long> getIds(void);
        std::vector<std::string> getGuids(void);



    };
}

#endif