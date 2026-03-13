#include "layersHelper.h"

namespace Layers
{
    std::string  
    layersHelper::
    getFileGuid(void) const
    {
        return (fileGuid);
    }

    layersHelper::
    layersHelper()
    {	
        layers = layersAPI::getAllLayers();	
    }

    layersHelper::
	layersHelper(const std::string& _fileGuid)
	{
        fileGuid = _fileGuid;
    }

    std::string
    layersHelper::
    createNewLayer(void)
    {
    #ifdef CREATE_NEW_LAYER_DBG
        BEGIN()
    #endif

        std::string ret =  layersAPI::createNewLayer();
        layers = layersAPI::getAllLayers();//передёргиваем слои в контейнере, 
        //чтобы не случилось  так, что нового слоя там не будет.
    #ifdef CREATE_NEW_LAYER_DBG
        END()
    #endif
        return ret;	
    }

    void 
    layersHelper::
    setLayers(std::string fileGuid)	
    {
    #ifdef REFRESH_DBG
        BEGIN()
    #endif			
        auto _layers = layersAPI::getLayersByFileGuid(fileGuid);
        layers = *_layers; 
    #ifdef REFRESH_DBG
        std::ostringstream out_s; out_s << " _layers = " << ((_layers) ? layers.size() : 0 ) << std::endl;	
        LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,out_s);
        DBG_VIEW(layers);
        END();
    #endif			
    }

    void
    layersHelper::
    setLayers(void)
    {
	#ifdef SET_LAYERS_DBG
		BEGIN()
	#endif
    if (fileGuid.empty())
        throw std::runtime_error("uncknown fileGuid for layer getting");
    auto Tmp =  layersAPI::getLayersByFileGuid(fileGuid);
    if (Tmp)	
        layers = *Tmp; 
    #ifdef SET_LAYERS_DBG
    {
        DBG_VIEW();
        std::ostringstream out_s; out_s << " layers.size() = " << layers.size() << std::endl;	
        LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,out_s);
        END()
    }
    #endif	
    }
   
    bool layersHelper::Empty(void)
    {
        return (IsEmpty(layers));
    }

    std::optional <SQLCMD::types::ul_long>
    layersHelper::getIdByHash(SQLCMD::types::ul_long layerHash)
    {
        return ( getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long , SQLCMD::types::ul_long>
                        ("layerId","layerHash",layerHash));
    }

    std::optional <SQLCMD::types::ul_long>
    layersHelper::getHashById(SQLCMD::types::ul_long layerId)
    {
        return ( getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long , SQLCMD::types::ul_long>
                        ("layerHash","layerId",layerId)
                    );
    }
   
    std::optional <std::string> 
    layersHelper::getGuidById(SQLCMD::types::ul_long layerId)
    {
        return ( getlayerSomeThingRelationNameByName< SQLCMD::types::ul_long , std::string >
                        ("layerGuid","layerId",layerId)
                    );
    }

    std::optional <SQLCMD::types::ul_long> 
    layersHelper::getIdByGuid(std::string  guid)
    {
        return ( getlayerSomeThingRelationNameByName< std::string , SQLCMD::types::ul_long >
                        ("layerId","layerGuid",guid)
                    );
    }
   

    SQLCMD::types::ul_long
    layersHelper::getFirstLayerId(void)
    {
        return (getFirstLayer<SQLCMD::types::ul_long>("layerId"));
    }

    SQLCMD::types::ul_long
    layersHelper::getLastLayerId(void)
    {
        return (getLastLayer<SQLCMD::types::ul_long>("layerId"));
    }

    #define SET_LAYERS_DBG

   
    void
    layersHelper::resetLayers(void)
    {
       layers = layersAPI::getAllLayers();//передёргиваем слои в контейнере, 
     }

    std::vector<SQLCMD::types::ul_long> layersHelper::getHashes(void)
    {
        return ( getlayerSomeThingByName< std::vector<SQLCMD::types::ul_long> >("layerHash") );
    }

}