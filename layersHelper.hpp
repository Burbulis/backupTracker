
#define _DEBUG_UPDATE_RECORDS_ON_

#ifndef __LAYERSHELPER__
#include "SqClass.h"
#include "string"
#include "sql/sqlcmdType.hpp"
#include "dbquery.hpp"
#include "layers_API.hpp"
#include "datasetAPI.hpp"
#define __LAYERSHELPER__


namespace LayersIO
{
	struct queryInfo
    {
        SQLCMD::types::ul_long idx;
        SQLCMD::types::ul_long layerHash;
		std::string blockGuid;
         
		 queryInfo(SQLCMD::types::ul_long layerHash,SQLCMD::types::ul_long _idx,std::string _blockGuid)
         :layerHash(layerHash),idx(_idx),blockGuid(_blockGuid)
        {};
    };
	using queryInfo_seq = std::vector<queryInfo>;

    struct layersHelper
	{
		enum mode
		{
			ONLY_MAX,
			ONLY_MIN,
			ALL,
		};

private:
	/// @brief вернуть соответствующий параметр слоя по имени
	/// @tparam T -соответствующий контейнер, для заданного параметра слоя
	/// @param field - имя парметра из таблицы layers.
	/// @return соответствующий контейнер,в данном случае строчный
	 	//static	
		DB::my_recordSet layers;
		DB::my_recordSet::iterator It;
		enum
		{
			EMPTY_CONST_INTEGRAL = 1110111
		};
		const static std::string EMPTY_CONST_STR;
        std::string fileGuid;

public:
	

		template
		<
			typename T
		>
		static typename std::enable_if< std::is_integral<T>::value, T >::type
		EMPTY()
		{
			return ( EMPTY_CONST_INTEGRAL );
		}

		template
		<
			typename T
		>
		static typename std::enable_if< !std::is_integral<T>::value, T >::type
		EMPTY()
		{
			return (EMPTY_CONST_STR);
		}


		template
		<
			typename T
		>
		static
		bool IsEmpty(const T& t)
		{
			return (t == EMPTY_CONST_INTEGRAL);
		}

		static  
		bool  IsEmpty(const std::string& t)
		{
			return (!t.compare(EMPTY_CONST_STR));
		}

		layersHelper(const std::string& _fileGuid):fileGuid(_fileGuid)
		{}
		
		std::string getFileGuid(void) const
		{
			return (fileGuid);
		}

		void setLayers(void)
		{
		#ifdef _DEBUG_UPDATE_RECORDS_ON_
			BEGIN()
		#endif
			if (fileGuid.empty())
				throw std::runtime_error("uncknown fileGuid for layer getting");
			auto Tmp =  layersAPI::getLayersByFileGuid(fileGuid);
			if (Tmp)	
				layers = *Tmp; 
			#ifdef _DEBUG_UPDATE_RECORDS_ON_
				DBG_VIEW();
				_LOG::out_s << " layers.size() = " << layers.size() << std::endl;	
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
				END()
			#endif	
		}


		static
	    bool IsEmpty(const  DB::my_recordSet& t)
    	{
       		return (!t.begin()->first.compare(DB::_QUERY_RESULT_IS_EMPTY)) ;
    	}

		layersHelper()
		{	
			layers = layersAPI::getAllLayers();	
		}

		bool IsEmpty()
		{
			return (IsEmpty(layers));
		}

		std::string
		createNewLayer(void)
		{
			std::string ret =  layersAPI::createNewLayer();
			layers = layersAPI::getAllLayers();//передёргиваем слои в контейнере, 
		   //чтобы не случилось  так, что нового слоя там не будет.
			return ret;	
		}

		void refresh(std::string fileGuid)	
		{
			auto _layers = layersAPI::getLayersByFileGuid(fileGuid);
			//if (!_layers)
			//{
			//	std::ostringstream out_s;
			//	out_s << " getLayersByFileGuid( " << fileGuid << ")" << std::endl;
			//	throw std::runtime_error(out_s.str())); 
			//}
			layers = *_layers; 
		}
		void refresh(void)
		{
			layers = layersAPI::getAllLayers();//передёргиваем слои в контейнере, 
		}

		template
		<
			typename T
		>
		typename std::enable_if< !std::is_integral<typename T::value_type>::value, T >::type
		getlayerSomeThingByName(std::string field)
		{
			return layers[field]; 
		}

	/// @brief вернуть соответствующий параметр слоя по имени
	/// @tparam T -соответствующий контейнер, для заданного параметра слоя
	/// @param field - имя парметра из таблицы layers.
	/// @return соответствующий контейнер,в данном случае цифровой 
		template
		<
			typename T
		>
		typename std::enable_if< std::is_integral<typename T::value_type>::value, T >::type
		getlayerSomeThingByName(std::string field)
		{
	#ifdef _DEBUG_UPDATE_RECORDS_ON_
			_LOG::out_s << "++getlayerSomeThingByName(" << field <<")" << std::endl;	
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		//}
				DBG_VIEW();
	#endif			
			return (DataSetAPI::getSomeThingByName<T>(field,layers));
		}


		template
		<
			typename T,  //u_long
			typename R //string
		>
		std::optional<R>
		getlayerSomeThingRelationNameByName(std::string relField,std::string byField,T byValue)
		{
	#ifdef _DEBUG_UPDATE_RECORDS_ON_
			BEGIN()	
			_LOG::out_s << " byField = " << byField << std::endl;	
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		//}
	
	#endif
			if ((layers[byField].empty()))
			{
	#ifdef _DEBUG_UPDATE_RECORDS_ON_
			_LOG::out_s << " byField = " << byField << std::endl;	
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"layers[byField].empty()");	
	#endif
			return std::nullopt;	
			}
			if ((layers[relField].empty()))
			{
	#ifdef _DEBUG_UPDATE_RECORDS_ON_
			_LOG::out_s << " relField = " << relField << std::endl;	
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"layers[relField].empty()");	
			for (auto f:layers)
			{
				_LOG::out_s << " f.first = " << f.first << " f.second.size()" << f.second.size() << std::endl;	
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			}
	#endif
			return std::nullopt;		
			}

			return (DataSetAPI::getSomeThingRelationNameByName<T,R>(relField,byField,byValue,layers));
		}

		template
		<
			typename T
		>
		T getLastLayer(const std::string& relfield)
		{
			#ifdef _DEBUG_UPDATE_RECORDS_ON_		
				_LOG::out_s << " ++layerHelper::getLastLayer('" << relfield << "')" << std::endl;	
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			#endif
			const std::string byField = std::string("layerId"); 
			std::vector<SQLCMD::types::ul_long> v_layers = getlayerSomeThingByName< 
					std::vector<SQLCMD::types::ul_long> 
				>(byField);			
			auto LayerIt = std::max_element(v_layers.begin(),v_layers.end(),[](SQLCMD::types::ul_long lval,SQLCMD::types::ul_long rval){
					return (lval < rval);
				});	
			T ret = *LayerIt;
			#ifdef _DEBUG_UPDATE_RECORDS_ON_		
				_LOG::out_s << " --layerHelper::getLastLayer() ret = " << ret <<  std::endl;	
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			#endif
			return ( (!relfield.compare(byField))? ret : 
			 			*getlayerSomeThingRelationNameByName<T,T>(relfield,byField,ret)
						);
		}


		template
		<
			typename T
		>
		T getFirstLayer(const std::string& relfield)
		{
			#ifdef _DEBUG_UPDATE_RECORDS_ON_		
				_LOG::out_s << " ++layerHelper::getFirstLayer('" << relfield << "')" << std::endl;	
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			#endif

			const std::string byField = std::string("layerId"); 
			std::vector<SQLCMD::types::ul_long> v_layers = getlayerSomeThingByName< 
					std::vector<SQLCMD::types::ul_long> 
				>(byField);			

			auto LayerIt = std::min_element(v_layers.begin(),v_layers.end(),[](SQLCMD::types::ul_long lval,SQLCMD::types::ul_long rval){
					return (lval < rval);
				});	
			T ret = *LayerIt;	
			#ifdef _DEBUG_UPDATE_RECORDS_ON_		
				_LOG::out_s << " --layerHelper::getFirstLayer()  ret = " << ret  << std::endl;	
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			#endif
			return ( (!relfield.compare(byField))? ret : 
			 			*getlayerSomeThingRelationNameByName<T,T>(relfield,byField,ret) );
		}

		#ifdef _DEBUG_UPDATE_RECORDS_ON_		

		static
		void DBG_VIEW(DB::my_recordSet& src)
		{
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING," ++DBG_VIEW");	
			for (auto s:src)
			{
				_LOG::out_s << "fieldName => " << s.first << std::endl;
				for (auto i:s.second)
					_LOG::out_s << "data = " << i << std::endl;
				LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
			}
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING," --DBG_VIEW");	

		}

		static
		void DBG_VIEW(DB::SQlite& sqlPerform)	
		{
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING," ++DBG_VIEW(1)");			
			auto rc = sqlPerform.getDataSet();
			auto fields = sqlPerform.getFieldNames();
			for(	auto field : fields )
			{
				_LOG::out_s <<"[" << field << "]->" << std::endl;
				std::vector< std::string > tmp = rc[field];
				_LOG::out_s <<	LOGHTML::showSeq(tmp).str();	
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			}
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING," --DBG_VIEW(1)");	
		}

	
		void DBG_VIEW(void)
		{
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING," ++DBG_VIEW(0)");	
			for (auto l:layers)
			{
			  _LOG::out_s << "fields = " << l.first << std::endl;
			  for (size_t i =0; i < l.second.size() ; ++i)
			  	 _LOG::out_s << "data = " << l.second[i] << std::endl;
  			  LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			}
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING," --DBG_VIEW(0)");	
		}



		#endif
	};
    const std::string layersHelper::EMPTY_CONST_STR = "NONE";

}
#endif