#ifndef __DATASETAPI__
#define __DATASETAPI__

#include <optional>
#include <string>
#include <type_traits>
#include "SqClass.h"

namespace DataSetAPI
{
	template <typename T, 
        typename R>	std::optional<R> 
	getSomeThingRelationNameByName(const std::string relField,const std::string byField,T byValue,DB::my_recordSet dataSet);
	template<typename T>typename std::enable_if< !std::is_integral<typename T::value_type>::value, T >::type
	getSomeThingByName(std::string field,const DB::my_recordSet& records);
	template<typename T>typename std::enable_if< std::is_integral<typename T::value_type>::value, T >::type
	getSomeThingByName(std::string field,const DB::my_recordSet& records);


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

	/// @brief вернуть соответствующий параметр слоя по имени
	/// @tparam T -соответствующий контейнер, для заданного параметра слоя
	/// @param field - имя парметра из таблицы layers.
	/// @return соответствующий контейнер,в данном случае цифровой 
		template
		<
			typename T
		>
		typename std::enable_if< !std::is_integral<typename T::value_type>::value, T >::type
		getSomeThingByName(std::string field,const DB::my_recordSet& records)
		{
			return ( const_cast<DB::my_recordSet&>(records)[field] );
		}

	template
		<
			typename T
		>
		typename std::enable_if< std::is_integral<typename T::value_type>::value, T >::type
		getSomeThingByName(std::string field,const DB::my_recordSet& records)
		{
			std::vector<std::string> tmp = const_cast<DB::my_recordSet&>(records)[field];;	
			T out_s;
			std::transform(tmp.begin(),tmp.end(),std::back_inserter(out_s),[](std::string str){
				return (std::stoull(str.c_str()));
			});
			return (out_s);
		}

    // @brief Вернуть соответствующий параметру ByValue из поля byField
	// параметр из поля relField связанный с ним по индексу.
	// В данном случае найти layerGuid соответствующий 
	// параметру из списка layerId, заданному параметром layerId
	//	layerGuid = 
	//getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long,std::string>("layerGuid","layerId",layerId); 		
	// @tparam T - тип параметра строк столбца таблицы byField 
	// @tparam R - тип найденного значения в столбце relField
	// @param relField - имя столбца возвращаемого связного значения  
	// @param byField  - имя столбца с которым по индексу связан relField
	// @param byValue  - значение которое будем искать в столбце с именем byField , для того , чтобы 
	// по индексу связать со столбцом relField и вернуть из него соответствующее индексу значение.
	// @return значение если всё успешно, 0 - если всё плохо.
		#define _GET_SOMETHING_RELATION_NAME_BY_NAME_
		template
		<
			typename T,  //u_long
			typename R //string
		>
		std::optional<R>
		getSomeThingRelationNameByName(std::string relField,std::string byField,T byValue,DB::my_recordSet dataSet)
		{
	#ifdef _GET_SOMETHING_RELATION_NAME_BY_NAME_	
			BEGIN()
	#endif
			if ((dataSet[byField].empty())||(dataSet[relField].empty()))
		   		return std::nullopt; 
			std::vector<T> byField_s =  DataSetAPI::getSomeThingByName< std::vector<T> >(byField,dataSet);
			std::vector<R> relField_s =  DataSetAPI::getSomeThingByName< std::vector<R> >(relField,dataSet);
	#ifdef _GET_SOMETHING_RELATION_NAME_BY_NAME_
			_LOG::out_s << " relField = "  << relField <<  "  byField = " << byField << " byField_s.size() = " << byField_s.size() << " relField_s.size() = " << relField_s.size()<< std::endl;
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			_LOG::out_s << "byField_s[0] = " << byField_s[0] << " relField_s[0] " << relField_s[0] << " byValue = " << byValue << std::endl;
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
	#endif
			auto It =std::find_if(byField_s.begin(),byField_s.end(),[byValue](const T& val){
					return (compare(byValue,val));
				});			

			if (It != byField_s.end())
			{
				size_t  n_dist = std::distance(byField_s.begin(),It);
	#ifdef _GET_SOMETHING_RELATION_NAME_BY_NAME_
			_LOG::out_s << "n_dist  = " << n_dist << std::endl;
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
	#endif
				auto It_ = relField_s.begin();	
				std::advance(It_,n_dist);
	#ifdef _GET_SOMETHING_RELATION_NAME_BY_NAME_
			_LOG::out_s << "*It_ = " << *It_ << std::endl;
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
            END()        
    #endif
				return ( *It_ );		
			}		
			END()
			return std::nullopt; 
		}
}
#endif