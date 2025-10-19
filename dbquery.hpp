#ifndef _DB_QRY
#define _DB_QRY

#include "SqClass.h"
#include "utils.hpp"
#include "DA_.hpp"
//#include "sql/sqlcmdType.hpp"
#include "sql/insert.hpp"
#include "sql/update.hpp"
#include "sql/create.hpp"
#include "sql/sqlGenerator.hpp"
#include "sql/sqlcopy.hpp"
#include "layers_API.hpp"
//#include "tempTable.hpp"
//#include "layersIO.hpp"

#define _DEBUG_UPDATE_RECORDS_ON_
//#define _DEBUG_CREATE_RECORDS_ON_
/*
Включение /отключение логгирования методов входящих в состав библиотеки.
*/
#define GET_FILE_SIZE_BY_GUID_LOG_ON   	//getFileSizeByGuid
#define GET_INFO_ABOUT_BLOCKS_LOG_ON   	//getInfoAboutBlocks
#define RECOEDSET_TO_QUERY_INFO_LOG_ON 	//recordSetToQueryInfo
#define GET_LAYERS_BY_BLOCKGUID_LOG_ON	//getLayersByBlockGuid
#define GET_BASE_LAYER_LOG_ON 			//getBaseLayer
#define GET_ALL_LAYERS_LOG_ON 			//getAllLayers
#define CREATE_NEW_LAYER_LOG_ON			//createNewLayer
#define CREATE_TEMP_TABLE_LOG_ON 		//createTempTable
#define GET_LAYERS_BY_FILE_GUID_LOG_ON	//getLayersByFileGuid
#define CHECKED_HASH_BY_LAYERS_LOG_ON	//checkedHashbyLayers
#define GET_SOMETHING_BY_NAME_LOG_ON	//getSomeThingByName
#define CREATE_BASE_LAYER_LOG_ON		//createBaseLayer
#define CHECK_CHANGED_BLOCKS_LOG_ON     //checkChangedBlocks
#define GET_FILES_AND_PATHS             //getfilesAndPaths
//#define GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON//getBlocksWithLayersById

namespace LAYER_QUERIES
{

 

   // typedef std::vector< LayersIO:: > queryInfo_seq;
    typedef std::map<std::string,size_t> layerByGuid;
	typedef std::map< std::string,SQLCMD::types::ul_long > hashByGuid;
	//typedef std::pair<std::string , bool >  blockSerialiseOn;
	

	//static	const SQLCMD::types::ul_long	getFileSizeByGuid(std::string fileGuid);
	static DB::my_recordSet  				getInfoAboutBlocks(const std::vector<std::string>& block_s);

	static void								createBaseLayer(void);
	static DB::my_recordSet  				getBaseLayer(void);
	static DB::my_recordSet  				getAllLayers(void);
	//static std::string	  	  				createNewLayer(std::string Uid = uid_sAndId_s::Newuid());
	//static std::optional<DB::my_recordSet> getLayersByFileGuid(std::string fileGuid);
	//static DB::my_recordSet	checkedHashbyLayers(const std::vector<SQLCMD::valdesc>& layerHashesForCheck);
	DB::my_recordSet getfilesAndPaths();

/*
	template
	<
		typename T
	>
	typename std::enable_if< std::is_integral<typename T::value_type>::value, T >::type
		getSomeThingByName(std::string field,DB::my_recordSet& records);

	template
	<
		typename T
	>
	typename std::enable_if< !std::is_integral<typename T::value_type>::value, T >::type
		getSomeThingByName(std::string field,DB::my_recordSet& records);
*/	
	// @brief Вернуть соответствующий параметру ByValue из поля byField
	// параметр из поля relField связанный с ним по индексу.
	// В данном случае найти layerGuid соответствующий 
	// параметру из списка layerId, заданному параметром layerId
	//	layerGuid = 
	//getSomeThingRelationNameByName<SQLCMD::types::ul_long,std::string>("layerGuid","layerId",layerId); 		
	// @tparam T - тип параметра строк столбца таблицы byField 
	// @tparam R - тип найденного значения в столбце relField
	// @param relField - имя столбца возвращаемого связного сначения  
	// @param byField  - имя столбца с которым по индексу связан relField
	// @param byValue  - значение которое будем искать в столбце с именем byField , для того , чтобы 
	// по индексу связать со столбцом relField и вернуть из него соответствующее индексу значение.
	// @param DB::my_recordSet dataSet -параметр содержащий в себе поля и их содержимое.	
	// @return значение если всё успешно, 0 - если всё плохо.

	template
	<
		typename T,  //u_long
		typename R //string
	>
	R getSomeThingRelationNameByName(const std::string& relField,const std::string& byField,T byValue,DB::my_recordSet dataSet);

	/*struct layersHelper
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
		static
		bool compare(std::string lval,std::string rval)
		{
			return ( !lval.compare(rval) );		
		}

		template
		<
			typename T
		>
		static
		bool compare(const T& lt,const T& rt)
		{
			return (lt == rt);
		}

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
			layers = LAYER_QUERIES::getLayersByFileGuid(fileGuid);		
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
			layers = LAYER_QUERIES::getAllLayers();	
		}

		bool IsEmpty()
		{
			return (IsEmpty(layers));
		}

		std::string
		createNewLayer(void)
		{
			std::string ret =  LAYER_QUERIES::createNewLayer();
			layers = LAYER_QUERIES::getAllLayers();//передёргиваем слои в контейнере, 
		   //чтобы не случилось  так, что нового слоя там не будет.
			return ret;	
		}

		void refresh(std::string fileGuid)	
		{
			layers = LAYER_QUERIES::getLayersByFileGuid(fileGuid);
		}
		void refresh(void)
		{
			layers = LAYER_QUERIES::getAllLayers();//передёргиваем слои в контейнере, 
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
	
	#endif			
			DBG_VIEW();
			return (getSomeThingByName<T>(field,layers));
		}


		template
		<
			typename T,  //u_long
			typename R //string
		>
		R
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
				return EMPTY<R>(); 
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
				return EMPTY<R>(); 
			}

			return (LAYER_QUERIES::getSomeThingRelationNameByName<T,R>(relField,byField,byValue,layers));
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
			 			getlayerSomeThingRelationNameByName<T,T>(relfield,byField,ret)
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
			 			getlayerSomeThingRelationNameByName<T,T>(relfield,byField,ret) );
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


	};*/
//	const std::string layersHelper::EMPTY_CONST_STR = "NONE";
	//DB::my_recordSet layersHelper::layers;

	/// @brief вернуть соответствующий параметр слоя по имени
	/// @tparam T -соответствующий контейнер, для заданного параметра слоя
	/// @param field - имя парметра из таблицы layers.
	/// @return соответствующий контейнер,в данном случае цифровой 
	/*	template
		<
			typename T
		>
		typename std::enable_if< !std::is_integral<typename T::value_type>::value, T >::type
		getSomeThingByName(std::string field,DB::my_recordSet& records)
		{
			std::vector<std::string> tmp = records[field];	
			return (tmp);
		}


	template
		<
			typename T
		>
		typename std::enable_if< std::is_integral<typename T::value_type>::value, T >::type
		getSomeThingByName(std::string field,DB::my_recordSet& records)
		{
			std::vector<std::string> tmp = records[field];	
			T out_s;
			std::transform(tmp.begin(),tmp.end(),std::back_inserter(out_s),[](std::string str){
				return (std::stoull(str.c_str()));
			});
			return (out_s);
		}

*/



	
	//static	bool IT_IS_FIRST_UPDATE(layersHelper& helper);
	
	template
	<
		typename bufferType
	>	
	static std::vector< std::pair<std::string,bool> > checkChangedBlocks(const bufferType& faBuffer);

/*
	template
	<
	typename bufferType,
	typename sqlCache
	>
	static
	std::optional<hashAndBlocks>	getBlocksWithLayersById(const bufferType & faBuffer,sqlCache& sql,ul_long fillerToken);
*/
/*
Попытка реализовать кэш для tempTable
*/


#define GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
/*
Захуярить лямбду как в примере!

#include <iostream>
#include <functional>

// Функция, принимающая лямбда-выражение в качестве аргумента
void processData(std::function<int(int, int)> operation, int a, int b) {
    int result = operation(a, b);
    std::cout << "Result: " << result << std::endl;
}

int main() {
    // Определение лямбда-выражения, которое складывает два числа
    auto add = [](int x, int y) {
        return x + y;
    };

    // Вызов функции, передавая лямбда-выражение
    processData(add, 5, 3);

    // Использование лямбда-выражения напрямую
    processData([](int x, int y) { return x * y; }, 4, 6);

    return 0;
}
*/


/*

	template
	<
	typename bufferType,
	typename sqlCache
	>
	static
	std::optional<hashAndBlocks>
	getBlocksWithLayersById(const bufferType &faBuffer,sqlCache& sql/*,ul_long fillerToken)
	{
	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		BEGIN()
	#endif 
		LayersIO::layersHelper  layersHelper;
		hashAndBlocks m_out;
	
//1)Находим блоки которые изменились, устанавливаем попутно флаг blockSerialiseOn ,
// этот флаг сообщает о том, что блок должен быть сериализован на диск
		std::vector< blockSerialiseOn >  checkedBlocks = checkChangedBlocks<bufferType>(faBuffer);
		if (checkedBlocks.empty())
		{
	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
			LOGTOFILE(LOGHTML::messageType::WARNING,"checkedBlocks  is empty!");
	#endif		
			return (std::nullopt);
		}

	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
	
		std::vector<std::string> checked;
		std::transform(checkedBlocks.begin(),checkedBlocks.end(),std::back_inserter(checked),[](blockSerialiseOn& val){
				return (val.first);	
		});
		std::vector<std::string>::iterator _It = std::unique(checked.begin(),checked.end());
		checked.resize( std::distance(checked.begin(),_It));
		for (std::string s:checked)
		{
			_LOG::out_s << "changedBlocks[blockGuid] = " << s << std::endl; 
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		}
	#endif

	std::vector< std::string > checkBlocks; 
	std::transform(checkedBlocks.begin(),checkedBlocks.end(),std::back_inserter(checkBlocks),[faBuffer](const std::pair<std::string,bool>& str){
		return ( str.first );
	});
	// Генерирует SQL select fileGuid,layerHash,idx,blockGuid,layerId ....4 where in(blockGuid[0],...,blockGuid[N])
   DB::my_recordSet selected  = getInfoAboutBlocks(checkBlocks);

   std::vector<std::string> selectedBlocks = selected["blockGuid"];

   	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		for (std::string s:selectedBlocks)
		{
			_LOG::out_s << "selectedBlocks = " << s << std::endl; 
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		}
		_LOG::out_s << "selectedBlocks.size()  = " << selectedBlocks.size() << "checkBlocks.size() = "  <<  checkBlocks.size() << std::endl; 
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
	#endif

	std::vector<std::string>::iterator It_uniq = std::unique(selectedBlocks.begin(),selectedBlocks.end());
	selectedBlocks.resize( std::distance(selectedBlocks.begin(),It_uniq));
	
	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		for (std::string s:selectedBlocks)
		{
			_LOG::out_s << "selectedBlocks = " << s << std::endl; 
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		}
		_LOG::out_s << "selectedBlocks.size()  = " << selectedBlocks.size() << "checkBlocks.size() = "  <<  checkBlocks.size() << std::endl; 
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
	#endif

		std::string    layerGuid;
	//	std::optional<types::ul_long> layerHash = std::nullopt;
		std::vector<SQLCMD::types::ul_long> v_layers = layersHelper.getlayerSomeThingByName< 
				std::vector<SQLCMD::types::ul_long> 
			>("layerId");	
			//TODO:04.12.24		
			//А вот этот вот recordSetToQueryInfo - вообще правильный подход??
			// Может лучше std::map[layerHash] = struct(idx,blockGuid) ?
		//queryInfo_seq  qi_s        					 = recordSetToQueryInfo(selected);
		LayersIO::queryInfo_seq qi_s = LayersIO::recordSetToQueryInfo(selected);
	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		for (auto q:qi_s)
		{
			_LOG::out_s << "q.blockGuid = " << q.blockGuid<< " q.layerHash = " << q.layerHash << " q.idx = " << q.idx << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
		}
	#endif
		
		LayersIO::layerBuGuidOptional    layerByBlockGuid  = LayersIO::getLayersByBlockGuid(qi_s) // layersIO::getLayersByBlockGuid(qi_s);

	
		SQLCMD::types::ul_long max_layerId = layersHelper.getLastLayer<SQLCMD::types::ul_long>("layerId");
	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		_LOG::out_s  << "max_layerId =" << max_layerId << std::endl;
			_LOG::out_s << "v_layers.size() = " << v_layers.size() << std::endl;
		for (auto i:v_layers)
			_LOG::out_s << " i = " << i << std::endl;	
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif

	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		LOGTOFILE(LOGHTML::messageType::NOTICE,"IT_IS_NOT_FIRST_UPDATE!");
		_LOG::out_s <<   " layers.size  = " << v_layers.size() <<  std::endl;
		for (auto l:v_layers)
			_LOG::out_s <<   "layer = " << l <<  std::endl;
		LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
		LOGTOFILE(LOGHTML::messageType::NOTICE,"for( dbg:lbg )");
		if (layerByBlockGuid)
		{
		layerByGuid lbg = *layerByBlockGuid;
		for (auto dbg:lbg)
			_LOG::out_s << dbg.first << ":" << dbg.second << " " << std::string((dbg.second == max_layerId)?"max_layerId - DETECTED":"max_layerId - NOT_DETECTED") << std::endl;
		LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
		}
		else
			LOGTOFILE(LOGHTML::messageType::NOTICE,"LBG_IS_EMPTY");


	#endif
		/*
		Ситуация обратная IT_IS_FIRST_UPDATE,
		мало того что обновления были, так они были последними,
		это финальный слой, поэтому нужно создать следующий слой для 
		дальнейшей истории.
		
		
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		_LOG::out_s << "max_layer_hash = " << max_layer_hash << " max_layerId = " << max_layerId  << std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
#endif	


	/*	if (
	//		std::any_of(layerByBlockGuid->begin(),layerByBlockGuid->end(),[max_layerId](const std::pair< std::string,size_t >& val)
	//		{
	//			return ( max_layerId == val.second );		
	//		}
	//	)
	//	)
	//	{
			//std::string Uid =  layersHelper.createNewLayer();
			//Хэш нового максимального слоя, для добавления новых блоков.
			SQLCMD::types::ul_long max_layer_hash = layersHelper.getLastLayer<SQLCMD::types::ul_long>("layerHash");	
			SQLCMD::types::ul_long new_layer_hash = utils::minicache::str_hash( layersHelper.createNewLayer() ) / 100000;
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
			_LOG::out_s << "max_layer_hash =" << max_layer_hash << " new_layer_hash = " << new_layer_hash <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
#endif
			TempObjects::createTempTable( sql , faBuffer.fileGuid , max_layer_hash , new_layer_hash);
	//	}
	//	else
	//	{
		//	const std::string fileGuid = std::string(faBuffer->buff.md.fileGuid);
			LAYER_QUERIES::layersHelper lh(faBuffer.fileGuid);
			lh.setLayers();
			SQLCMD::types::ul_long layerId = lh.getLastLayer<SQLCMD::types::ul_long>("layerId");
			SQLCMD::types::ul_long max_layer_hash_prev = lh.getLastLayer<SQLCMD::types::ul_long>("layerHash");
			lh.refresh();
			max_layer_hash =	lh.getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long,SQLCMD::types::ul_long>("layerHash","layerId",++layerId);
						
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		_LOG::out_s << "max_layer_hash_prev =" << max_layer_hash_prev <<  std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
#endif
			TempObjects::createTempTable(sql,faBuffer.fileGuid,max_layer_hash_prev,max_layer_hash);
		}	



#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		LOGTOFILE(LOGHTML::messageType::NOTICE,"checkedBlocks inspecting:");
		_LOG::out_s << std::endl;
		for (auto i:checkedBlocks)
			_LOG::out_s << "blockGuid =" << i.first <<  std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
#endif
//TODO:
//переименовать ничего не значащую переменную i, во чтото более адекватное.		
		for (LAYER_QUERIES::blockSerialiseOn i:checkedBlocks)
		{
			size_t layerId = (*layerByBlockGuid)[i.first];
			size_t old_layerId = layerId;
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
				_LOG::out_s << "i = " << i.first << " layerByBlockGuid = " << (*layerByBlockGuid)[i.first] << " max_layer_hash=" << max_layer_hash << " layerId = " << layerId << std::endl;
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
#endif

#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
			_LOG::out_s << "layerId =" << layerId << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
#endif
		  	SQLCMD::types::ul_long layerHash =layersHelper.getlayerSomeThingRelationNameByName< SQLCMD::types::ul_long , SQLCMD::types::ul_long >("layerHash","layerId",++layerId);				
			if (!LAYER_QUERIES::layersHelper::IsEmpty(layerHash))
			{
				m_out[ layerHash ].push_back(i);
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
				_LOG::out_s << "m_out[" << layerHash << "] = " <<  i.first  << std::endl;
				LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
#endif
			}
			else
			{
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"++layerId NOT_FOUND!");
#endif
				//return (std::nullopt);
			}
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
			END();
#endif

			return (m_out);
		}


#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		END();
#endif 

		return (m_out);
	}
*/

	/// @brief check for first update event
	/// @param  none
	/// @return if this first update return true / otherwise false.
	//static
	//bool IT_IS_FIRST_UPDATE(layersHelper& helper)
	//{	//layers = getAllLayers();
//		return (1 == helper.getlayerSomeThingByName<std::vector<SQLCMD::types::ul_long> >("layerId").size());//(layers["layerId"].size() == 1);
//	}


	/// @brief return list of layer-s by fileGuid
	/// @param fileGuid - fileGuid of file that was selected
	/// @return recordSet containing the strings and fileId-s values.
/*
	static 
	std::optional<DB::my_recordSet>				
	getLayersByFileGuid(std::string fileGuid)
	{
		#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
			BEGIN()
		#endif

		std::string selectQueryStr="";
   		DB::SQlite  sqlPerform("db_buckets");	

		{
			
			SQLCMD::selector_type m{
				{ "fileGuid", SQLCMD::valdesc(fileGuid, std::vector<std::string>({ "buckets_and_blocks" }), SQLCMD::valdesc::WHERE_ON, false) },
				{ "layerHash"   ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,true) },
				{ "blockHash"   ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,false) },
			};

			m["layerHash"].setAsPredicate(SQLCMD::valdesc::DISTINCT);


			SQLCMD::Generator< SQLCMD::selector_type_v,
				SQLCMD::Select< SQLCMD::InnerJoinId0> >
				_select("buckets_and_blocks", { "blocks" },SQLCMD::get_ordered(m));
		
			
			SQLCMD::wrapper<SQLCMD::selector_type>::add(
				{
					{"blockHash", SQLCMD::valdesc() },
					{"blockHash", SQLCMD::valdesc()}
				}
			);
			_select.getCmdObject().linkJoin({ "blockHash" }, { "blockHash" });
			selectQueryStr= _select();
		}
		try
		{
			sqlPerform.execute(selectQueryStr);
			#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
				_LOG::out_s << "selectQueryStr = " << selectQueryStr  << std::endl;
				LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
			#endif
		}
		catch(const std::exception& e)
		{
	#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
			_LOG::out_s << "Exception e.what() = " <<  e.what()  << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif
			getc(stdin);
			return (std::nullopt);
		}
	
		#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"THE_NEXT_STAGE!");
		#endif			

		if ( sqlPerform.queryResultIsEmpty() )
		{
			#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"QUERY IS EMPTY!!!!");
			#endif			
			return (std::nullopt);
		}
	#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"TRY TO RETRIEVE RECORDSET!");
	#endif

		auto recordSet = sqlPerform.getDataSet();
		
	#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
			_LOG::out_s << "recordSet.size() = " <<  recordSet.size()  << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif


		std::vector<SQLCMD::types::ul_long> layerHashes =layersAPI::getSomeThingByName< std::vector<SQLCMD::types::ul_long> >("layerHash",recordSet);
		std::vector<SQLCMD::valdesc> inCollection;
		std::transform(layerHashes.begin(),layerHashes.end(),std::back_inserter(inCollection),[](SQLCMD::types::ul_long arg){
			return (SQLCMD::valdesc(arg));
		});
		#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
			END()
		#endif
		return (checkedHashbyLayers(inCollection));

	}

	/// @brief 	//scope subblock for check actual layers selected from buckets in layer table.	
	/// @param layerHashesForCheck //layerHash's for checking 
	/// @return dataset .
	static
	DB::my_recordSet
	checkedHashbyLayers(const std::vector<SQLCMD::valdesc>& layerHashesForCheck)
	{
		SQLCMD::selector_type m_layers = 
		{
			{"layerId",   SQLCMD::valdesc()},
			{ "layerGuid", SQLCMD::valdesc()},
			{ "layerHash", SQLCMD::valdesc(0,SQLCMD::valdesc::WHERE_IN)}
		};
		SQLCMD::Generator < SQLCMD::selector_type_v, SQLCMD::Select<> > selectFromLayers("layers", SQLCMD::get_ordered(m_layers));
		selectFromLayers.getCmdObject().addInParam(layerHashesForCheck);
		std::string sq_layer = selectFromLayers();	
		DB::SQlite  _sqlPerform("db_buckets");	
		try
		{
		#ifdef CHECKED_HASH_BY_LAYERS_LOG_ON
			_LOG::out_s << "sq_layer = " << sq_layer << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);	
		#endif	
			_sqlPerform.execute(sq_layer);
		}
		catch(const std::exception& e)
		{
			_LOG::out_s << "Exception e.what() = " <<  e.what()  << std::endl;
			getc(stdin);
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			return (DB::EMPTY());
		}	
		auto _fields = _sqlPerform.getFieldNames();
		DB::my_recordSet _recordSet = _sqlPerform.getDataSet();
		#ifdef CHECKED_HASH_BY_LAYERS_LOG_ON
				//layersHelper::DBG_VIEW(_sqlPerform);
			
			END();
		#endif
		return (_recordSet);
	}

*/
	



	DB::my_recordSet
	getfilesAndPaths()
	{
		#ifdef GET_FILES_AND_PATHS
		LOGTOFILE(LOGHTML::messageType::NOTICE,"++getfilesAndPaths()");
		#endif
		SQLCMD::Generator< SQLCMD::selector_type_v,
        SQLCMD::Select<> >
        sys_select("system_ids", 
            SQLCMD::get_ordered(
				SQLCMD::selector_type   {
                        {"pathFile"    ,SQLCMD::valdesc()},
                        {"fileName"    ,SQLCMD::valdesc()},
                        {"fileGuid"    ,SQLCMD::valdesc()},
                	}
                )
        );
		std::string s = sys_select();
		#ifdef GET_FILES_AND_PATHS
			_LOG::out_s << "s = " <<  s  << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
		#endif
		DB::SQlite  sqlPerform("db_buckets");	
		sqlPerform.execute(s);
		DB::my_recordSet rs = sqlPerform.getDataSet();
		#ifdef GET_FILES_AND_PATHS
			_LOG::out_s << "rs.size() = " << rs.size()  << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);

			for (auto r:rs)
			{
				_LOG::out_s << "r.first = " <<  r.first  << std::endl;
				LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
			}
			LOGTOFILE(LOGHTML::messageType::NOTICE,"--getfilesAndPaths()");
		#endif
		return (rs);
	}

	/*/// @brief size file from base by fileGuid
	/// @param fileGuid input parametr 
	/// @return size of file
	std::optional<SQLCMD::types::ul_long>
	getFileSizeByGuid(std::string fileGuid)
	{
		/*"layerId","layerHash"}
		#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
			BEGIN()
		#endif
		layersHelper _helper(fileGuid);
		_helper.setLayers();
		if (_helper.IsEmpty())
		{
			_LOG::out_s << "getFileSizeByGuid-error fileGuid = " <<  fileGuid << "not found!" << std::endl;
			throw std::runtime_error( _LOG::out_s.str() );			
		}
	//	DB::my_recordSet layers = getLayersByFileGuid<> ();
		
		const SQLCMD::types::ul_long LAYER_HASH_ID = _helper.getLastLayer<SQLCMD::types::ul_long>(std::string("layerHash"));
		//	(!_helper.getlayerSomeThingByName< std::vector<SQLCMD::types::ul_long> >("layerHash").empty())?
		//	_helper.getlayerSomeThingByName< std::vector<SQLCMD::types::ul_long> >("layerHash")[0]:0;

		SQLCMD::selector_type m = {
									{ "fileGuid" 	, SQLCMD::valdesc(fileGuid,SQLCMD::valdesc::WHERE_ON,false)},
									{ "blockHash"	, SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,false)},
									{ "size"     	, SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,true)},
									{ "layerHash"   , SQLCMD::valdesc(LAYER_HASH_ID ,std::vector<std::string>({"blocks"}) 	,SQLCMD::valdesc::WHERE_AND,false)},
			};
		m["size"].setAsPredicate(SQLCMD::valdesc::SUM);
		SQLCMD::Generator < SQLCMD::selector_type_v, SQLCMD::Select< SQLCMD::InnerJoinId0 > > _select("buckets_and_blocks", { "blocks" }, SQLCMD::get_ordered(m));
		SQLCMD::wrapper<SQLCMD::selector_type>::add(
			{
				{"blockHash", SQLCMD::valdesc()}
			}
		);
		_select.getCmdObject().linkJoin({ "blockHash" }, { "blockHash" });
		auto s = _select();
		DB::SQlite  sqlPerform("db_buckets");		
		#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
			_LOG::out_s << "s = " <<  s  << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
		#endif
		try{
		sqlPerform.execute(s);
		}
		catch(const std::exception& e)
		{
			_LOG::out_s << "Exception e.what() = " <<  e.what()  << std::endl;
			getc(stdin);
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);	
		
			return (layersHelper::EMPTY<SQLCMD::types::ul_long>());
		}
		DB::my_recordSet rs = sqlPerform.getDataSet();
		if (DB::IsEmpty(rs))
			throw std::runtime_error("file size calculation error!");

		#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
			_LOG::out_s << "rs.size() = " <<  rs.size()  << std::endl;
			for (auto i:rs)
			{
				_LOG::out_s << "rs.first = " <<  i.first << "rs.second.size() = " << i.second.size() << std::endl;
				LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
			}
		#endif
		SQLCMD::types::ul_long size = 0;
		try
		{
			size = getSomeThingByName< std::vector< SQLCMD::types::ul_long> >("size",rs)[0];
			#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
				_LOG::out_s << "size = " <<  size << std::endl;
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);			
			#endif
		}
		catch(const std::exception& e)
		{
			#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
				_LOG::out_s << "Exception catched = " << e.what() << std::endl;
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			#endif
		}
		#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
			END()
		#endif
		return ( size );
	}*/


    /// @brief если базового слоя ещё нет,создать его!
    /// @param  без параметров
    static
	void
	createBaseLayer(void)
	{
	#ifdef CREATE_BASE_LAYER_LOG_ON
		LOGTOFILE(LOGHTML::messageType::WARNING , "++CREATE BASE_LAYER" );
		//LOG("++CREATE BASE_LAYER");
	#endif
		//using namespace SQLCMD;
        std::string Uid = uid_sAndId_s::Newuid();
		SQLCMD::types::ul_long layerHash = utils::minicache::str_hash( Uid ) / 100000 ;
		
	    SQLCMD::inserter_type m ={
			{ "layerGuid",SQLCMD::valdesc(Uid) }, 
			{ "layerHash",SQLCMD::valdesc(layerHash) }, 
			{ "layer_start",SQLCMD::valdesc(SQLCMD::types::timeDef())}, 
			{ "layerId",SQLCMD::valdesc(SQLCMD::types::ul_long(1),SQLCMD::valdesc::WHERE_ON)}
		};
	    SQLCMD::Generator< SQLCMD::inserter_type, SQLCMD::InsertIfNotExist> layers(std::string("layers"),m);
	
		
			std::string layers_ = layers();	
	#ifdef CREATE_BASE_LAYER_LOG_ON	
			_LOG::out_s << "layers = " << layers_ << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
	#endif
      	DB::SQlite  sqlPerform("db_buckets");	
		sqlPerform.prepare(layers_);
		sqlPerform.execute();
	
	#ifdef CREATE_BASE_LAYER_LOG_ON
	        LOGTOFILE(LOGHTML::messageType::WARNING,"--CREATE BASE_LAYER");
	#endif
	}



/*
	создать новый слой.

	static
	std::string
	createNewLayer(std::string Uid)
	{
		#ifdef  CREATE_NEW_LAYER_LOG_ON
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"++createNewLayer");
		#endif
        DB::SQlite  sqlPerform("db_buckets");
        
		SQLCMD::types::ul_long layerHash = utils::minicache::str_hash( Uid ) / 100000 ;

	    SQLCMD::Generator< SQLCMD::inserter_type, SQLCMD::InsertIfNotExist> layers(std::string("layers"),
        {
        	{ "layerGuid",SQLCMD::valdesc(Uid) }, { "layerHash",SQLCMD::valdesc(layerHash) },{ "layer_start",SQLCMD::valdesc(SQLCMD::types::timeDef())} ,{ "##layerGuid",SQLCMD::valdesc(std::string(Uid),SQLCMD::valdesc::WHERE_ON,false)}
        });
		std::string s = layers();

		#ifdef  CREATE_NEW_LAYER_LOG_ON
			_LOG::out_s << "s = " << s << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif

		sqlPerform.execute(s);

		#ifdef  CREATE_NEW_LAYER_LOG_ON
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"--createNewLayer");
		#endif	

		return (Uid);
	}
*/
   /// @brief Возвращает атрибуты базового слоя. (layerGuid,layerId,layerHash)
   /// @param  -//-
   /// @return  DB::my_recordSet с одним единственным элементом. У которого минимальны layerId , 
   //соответственно он и есть описатель базового слоя. 
   static
   DB::my_recordSet
   getBaseLayer(void)
   {
		#ifdef  GET_BASE_LAYER_LOG_ON
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"++getBaseLayer()");
		#endif
		DB::my_recordSet ret;
		SQLCMD::selector_type m = { 
						{ "layerGuid", SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)},
						{ "layerId"  , SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)},
						{ "layerHash"   , SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)}
						};
	
		m["layerId"].setAsPredicate(SQLCMD::valdesc::MIN);
		SQLCMD::Generator<
			SQLCMD::selector_type_v,
			SQLCMD::Select<>
		>selectObjChecks( "layers", SQLCMD::get_ordered(m));
		
		DB::SQlite  sq("db_buckets");
		try
		{
			sq.execute(selectObjChecks());
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return (sq.getDataSet());
		}
		SQLCMD::valdesc::clearInstanceCounter();
		ret = sq.getDataSet(); 
		#ifdef  GET_BASE_LAYER_LOG_ON
			_LOG::out_s << "ret.second.size() = " << ret.size() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
		return (ret);
   }

   /// @brief возвращает полную информацию по всем слоям.
   /// @param  нет.
   /// @return  возвращает полную информацю по слоям layerGuid,layerId,layerHash в виде : DB::my_recordSet
 /*  static
   DB::my_recordSet
   getAllLayers(void)
	{
		#ifdef  GET_ALL_LAYERS_LOG_ON
			LOGTOFILE(LOGHTML::messageType::WARNING,"++getBaseLayer()");
		#endif
		DB::my_recordSet ret;
		SQLCMD::selector_type m = { 
						{"layerGuid", SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)},
						{"layerId", SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)},
						{"layerHash", SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)}
						};

		SQLCMD::Generator<
			SQLCMD::selector_type_v,
			SQLCMD::Select<>
		>selectObjChecks( "layers", SQLCMD::get_ordered(m));
		DB::SQlite  sq("db_buckets");
		try
		{
			std::string s = selectObjChecks();
		#ifdef _DEBUG_UPDATE_RECORDS_ON_
			_LOG::out_s << " SQL:: " << s << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
		#endif

			sq.execute(s);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return (ret);
		}
		SQLCMD::valdesc::clearInstanceCounter();
		ret = sq.getDataSet(); 
		#ifdef  GET_ALL_LAYERS_LOG_ON
			_LOG::out_s << "ret.second.size() = " << ret.size() << std::endl;
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		#endif
		return (ret);
	}
*/
    /// @brief  Генерирует SQL select fileGuid,layerHash,idx,blockGuid,layerId .... where in(blockChkSum[1],...,blockChkSum[N])
    /// @param block_s //список blockGuid блоков, информацию по которым нужно получить. 
    /// @return //информация передана в DB::my_recordSet, после исполнения SQL запроса.
   
   
  /* 
    DB::my_recordSet
    getInfoAboutBlocks(const std::vector<std::string>& block_s)
    {
		#ifdef GET_INFO_ABOUT_BLOCKS_LOG_ON	
				LOGTOFILE(LOGHTML::messageType::WARNING,"++getInfoAboutBlocks()");
		#endif  
			SQLCMD::Generator< SQLCMD::selector_type_v,
			SQLCMD::Select< SQLCMD::InnerJoinId0> >
			_select("buckets_and_blocks", { "blocks" },
			SQLCMD::get_ordered
			(
				{
					{"fileGuid"     ,SQLCMD::valdesc()},
					{"layerHash"    ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}), SQLCMD::selector_cmd_type::NONE),},
					{"blockHash"    ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}), SQLCMD::selector_cmd_type::NONE)},
					{ "idx"         ,SQLCMD::valdesc() },
					{ "blockGuid"   ,SQLCMD::valdesc(std::string(""),SQLCMD::valdesc::WHERE_IN)},
				}
			)
			);


			SQLCMD::wrapper<SQLCMD::selector_type>::add(
				{
					{"blockHash", SQLCMD::valdesc()}
				}
			);
			_select.getCmdObject().linkJoin({ "blockHash" }, { "blockHash" });
			std::vector<SQLCMD::valdesc> check_s;
			std::transform(block_s.begin(), block_s.end(), std::back_inserter(check_s), [](const std::string& str) {
				return (SQLCMD::valdesc(str));
				});
			_select.getCmdObject().addInParam(check_s);
            DB::SQlite  sq("db_buckets");
            std::string s = _select();
			#ifdef GET_INFO_ABOUT_BLOCKS_LOG_ON	
				_LOG::out_s << "s = " << s << std::endl; 
				LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			#endif  
			try
            {
                sq.execute(s);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            SQLCMD::valdesc::clearInstanceCounter();
            DB::my_recordSet selected = sq.getDataSet(); 
		#ifdef GET_INFO_ABOUT_BLOCKS_LOG_ON	
				LOGTOFILE(LOGHTML::messageType::WARNING,"--getInfoAboutBlocks()");
		#endif  
            return (selected);
    };//getInfoAboutBlocks
*/
   




	// Допилить поведение на случай пустого датасета!!!
	// вероятно надо возвращать std::optional<DB::my_recordSet>
	// чтобы на париться с исклуючением в случае пустого датасета.
    /// @brief для заданных контрольных сумм блоков, во входящем параметре , ищет в базе соответствующие им :
	/// blockHash,status,layyerHash или пустой вектор если контрольные суммы не ннайдены.
    /// @param const std::vector<SQLCMD::valdesc>& collect_for_check - список контрольных сум блоков подлежащих обнаружению в базе. 
	/// @param fileGuid -соответствующий fileGuid для поиска контрольльтных сумм.
    /// @return для заданных контрольными суммами блоков(если они контрольнные суммы обнаружены в таблице blocks ) , возвращает соответствующие им поля: 
	///status,blockHash,layerHash,chk согласно запросу ниже:
	///SELECT blocks.status,blocks.blockHash,blocks.layerHash,blocks.chk FROM blocks 
	///	inner join buckets_and_blocks on blocks.blockHash = buckets_and_blocks.blockHash 
	//where blocks.chk IN(269733645692229,,,,N) AND buckets_and_blocks.fileGuid like 'XXX' 
	//если ничего найдено небыло, возврашается std::nullopt.
	/*std::optional<DB::my_recordSet> 
	_getCheckedBlocks(const std::vector<SQLCMD::valdesc>& collect_for_check,std::string fileGuid)
	{
		SQLCMD::selector_type m = {
                            {"chk", SQLCMD::valdesc(0,SQLCMD::valdesc::WHERE_IN)},
                            { "fileGuid", SQLCMD::valdesc(fileGuid,std::vector<std::string>({"buckets_and_blocks"}), SQLCMD::valdesc::WHERE_AND,false)},
                            { "status", SQLCMD::valdesc()},
                            { "blockHash", SQLCMD::valdesc()},
                            { "layerHash", SQLCMD::valdesc()}
    	};

		SQLCMD::Generator< SQLCMD::selector_type_v,
			SQLCMD::Select< SQLCMD::InnerJoinId0> >
			selectObjChecks("blocks", { "buckets_and_blocks" }, SQLCMD::get_ordered(m)
				);

		SQLCMD::wrapper<SQLCMD::selector_type>::add(
			{
				{"blockHash", SQLCMD::valdesc() },
			}
		);
		selectObjChecks.getCmdObject().addInParam(collect_for_check);
		selectObjChecks.getCmdObject().linkJoin({ "blockHash" }, { "blockHash" });
		DB::SQlite  sq("db_buckets");
		try
		{
			std::string s_ = selectObjChecks(); 
			#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			_LOG::out_s << "s_ = " << s_ << std::endl; 
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			#endif      
 			sq.execute(s_);
			if (sq.queryResultIsEmpty())
				return (std::nullopt);
		}
		catch(const std::exception& e)
		{
			#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
				_LOG::out_s << "exception detected" << e.what() << std::endl; 
				LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			#endif   
			throw(e);
		}
		
		SQLCMD::valdesc::clearInstanceCounter();
		//if (sq.queryResultIsEmpty())
		//	throw(std::runtime_error("empty dataset!"));
		return (sq.getDataSet());
	//	DB::my_recordSet selected =  
	}*/

	//TODO :Заебенить это гавно в нормальный вид!
    /// @brief Поиск блоков которые подверглись изменению. 
    /// @tparam bufferType 
    /// @param параметр полученный от клиента. Содержащий информацию о блоках, статус изменения которых, необходимо проверить. 
    /// @return возвращаются пара blockGuid-ы блоков , которые подвергались изменению.То есть список тех блоков,которые подверглись изменению и bool который говорит о том,
	//является ли блок заполнителем или нет? Если заполнитель сериализация блока не происходит, в дальнейшем.
   /* template
    <
        typename bufferType
    >
	std::vector< blockSerialiseOn >
	checkChangedBlocks(
	 const	bufferType& faBuffer
	)
		//const std::unique_ptr<bufferType>& faBuffer)
	{
		#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			BEGIN()
		#endif
		std::string fileGuid = std::string(faBuffer.fileGuid); 
	    std::set<SQLCMD::types::ul_long> _hashes;
		enum{FIRST_ELEMENT=0};
		std::vector<SQLCMD::valdesc> collect_for_check;
		std::vector< blockSerialiseOn > ret;
		//const size_t counter = fa//utils::getCountOf(faBuffer.get());	
		const size_t blocksCount =  faBuffer.blocks.size();
        bool isFinal = false;
  
		#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			_LOG::out_s << "fileGuid = " << fileGuid  << std::endl; /*<< " counter = " << counter  
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		#endif   

		std::transform(faBuffer.blocks.begin(),faBuffer.blocks.end(),std::back_inserter(collect_for_check),[](const DA::tdataAttrib_s& val)
		{
			const std::string& blockGuid = std::string(val.blockGuid);
			const SQLCMD::types::ul_long& CHECK = val.chk;	
			if ((0 != CHECK)||(utils::isFiller<DA::tdataAttrib_s>(val)))//utils::isFiller<DA::tdataAttrib_s>(val)));
				return (SQLCMD::valdesc(CHECK));
		});

		std::transform(faBuffer.blocks.begin(),faBuffer.blocks.end(),std::back_inserter(ret),[](const DA::tdataAttrib_s& val)
		{
			const std::string& blockGuid = std::string(val.blockGuid);
			bool ret = !utils::isFiller<DA::tdataAttrib_s>(val);
			return ( std::pair<std::string,bool>(blockGuid,ret));
		});

		#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			_LOG::out_s << "collect_for_check.size() = " << collect_for_check.size() << "ret.size() = "  << ret.size() << std::endl; 
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			for (size_t i = 0; i < collect_for_check.size(); ++i)
				_LOG::out_s << "collect_for_check[" << i  << "] = " << collect_for_check[i].value << std::endl; 
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		#endif      
		
		std::optional<DB::my_recordSet> selected = _getCheckedBlocks(collect_for_check,fileGuid);
		
		if (!selected)
		{
			#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			LOGTOFILE(LOGHTML::messageType::NOTICE,"selected.empty()");
			END();
			#endif      
			return ret;
		}
		ret.clear();
    

// generate squence of block for update
	//	std::vector< blockSerialiseOn > ret;
		const size_t counter = faBuffer.blocks.size();
		for (size_t i = 0 ;	i < counter ; ++i)
		{	
			const std::string& blockGuid = std::string(faBuffer.blocks[i].blockGuid);
			const SQLCMD::types::ul_long& CHECK = faBuffer.blocks[i].chk;		
		  	const SQLCMD::types::ul_long& status = getSomeThingRelationNameByName<SQLCMD::types::ul_long,SQLCMD::types::ul_long>("status","chk",CHECK, *selected);
			const SQLCMD::types::ul_long& blockHash = getSomeThingRelationNameByName<SQLCMD::types::ul_long,SQLCMD::types::ul_long>("blockHash","chk",CHECK, *selected);
			const bool SERIALISER_ON = !utils::isFiller<DA::tdataAttrib_s>(faBuffer.blocks[i]);//utils::isFiller<bufferType>(ptr,blockGuid);
			if (
					((!layersHelper::IsEmpty(blockHash)) && (!layersHelper::IsEmpty(status)) || !SERIALISER_ON) && (!_hashes.count(CHECK))
			)		
			{
				_hashes.insert(CHECK);
				ret.push_back(std::pair<std::string,bool>( blockGuid , SERIALISER_ON));
	#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
				_LOG::out_s << " chk[" << i << "]=" << faBuffer.blocks[i].chk <<" [status = ]" << status << " blockHash = " << blockHash << "isFiller = " 
				<< isFiller << " ACCEPTED!" << std::endl;
				LOGTOFILE( LOGHTML::messageType::WARNING ,_LOG::out_s);
	#endif
			}
			else
			{
	#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
				_LOG::out_s << " chk[" << i << "]=" << faBuffer.blocks[i].chk << " blockHash = " << blockHash <<" NOT_ACCEPTED!" << std::endl;
				LOGTOFILE( LOGHTML::messageType::WARNING ,_LOG::out_s);
	#endif
			}
		}

	#ifdef CHECK_CHANGED_BLOCKS_LOG_ON		
		_LOG::out_s << "ret.size() = " << ret.size()  << std::endl;  
		LOGTOFILE(LOGHTML::messageType::WARNING , _LOG::out_s);
		if (!_hashes.empty())
		{
			_LOG::out_s << "_hashes.size() = " << _hashes.size()  << std::endl;  
			LOGTOFILE(LOGHTML::messageType::WARNING , _LOG::out_s);
		
		}
		else
		{
			_LOG::out_s << " _hashes is empty!" << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING , _LOG::out_s);
		}
		END()
	#endif
		return ret;
	}
*/


}//LAYER_QUERIES

#endif