#ifndef __SQL_GENERATOR__
#define __SQL_GENERATOR__
#include "sqlcmdType.hpp"
namespace SQLCMD
{
    selector_type_v
    get_ordered(const selector_type m)
    {
        selector_type_v v;
        std::copy(m.begin(),m.end(),std::back_inserter(v));
        std::sort(v.begin(),v.end(),[]
            (const SQLCMD::pair &l,const SQLCMD::pair& r)
            {
                return (l.second.current_instance < r.second.current_instance);
            }
        );
        return (v);
    }

    selector_type_v
    get_ordered(const multi_selector_type m)
    {
      selector_type_v v;
      std::copy(m.begin(), m.end(), std::back_inserter(v));
      std::sort(v.begin(), v.end(), []
      (const SQLCMD::pair& l, const SQLCMD::pair& r)
        {
          return (l.second.current_instance < r.second.current_instance);
        }
      );
      return (v);
    }



    selector_type_v
    type_v(const selector_type m)
    {
        selector_type_v v;
        std::transform(m.begin(), m.end(), std::back_inserter(v), [](SQLCMD::pair p) {
            return (p);
            });
        return (v);
    }

  
    template
        <
        typename T
        >
    struct is_normal_type
    {
        const static bool value = (std::is_same<T, SQLCMD::selector_type >::value   || 
                                   std::is_same<T, SQLCMD::selector_type_v>::value || 
                                   std::is_same<T, SQLCMD::inserter_type>::value);
    };


    template
        <
        typename T
        >
        struct sz_wrapper
    {
        template
            <
            typename T0
            >
            static typename std::enable_if< is_normal_type<T0>::value, size_t >::type tt_size(const T0& t)
        {
            return (t.size());
        };

        template
            <
            typename T1
            >
            static  typename std::enable_if< !is_normal_type<T1>::value, size_t >::type tt_size(const T1& t)
        {
            return (1);
        };

        static
        size_t
        get_size(const T& t)
        {  
            return (tt_size(t));
         //   return (sz);
        }

     //    private:
     //   static size_t sz;
    };
  //  template
   //     <
   //     typename T
     //   >
  //      size_t sz_wrapper<T>::sz;

    template
    <
        typename dataType,
        typename CmdExecutor
    >
    class Generator
    {
        size_t limit;
        CmdExecutor executedObject;
        std::string tmp;
        dataType value;

        template
            <
            typename T
            >
        static size_t get_size(T& t)
        {
            return (sz_wrapper<T>::get_size(t) );
        }

        public:
        Generator(std::string tableName,dataType value_):value(value_),
            executedObject(tmp,tableName, get_size(value)), limit(0)
        {
            executedObject.setEnd(get_size(value));
        }

        Generator(std::string tableName,std::vector<std::string> joinedTable,dataType value_):value(value_),
            executedObject(tmp,tableName,joinedTable,get_size(value)),limit(0)
        {
            executedObject.setEnd(get_size(value));
        }

        Generator(std::string tableName,dataType value_,size_t limit_):value(value_), 
            executedObject(tmp,tableName,value.size()),limit(limit_)
        {
            executedObject.setEnd(get_size(value));
        }
        
        CmdExecutor&
        getCmdObject()
        {
            return (executedObject);
        }
        
        template
            <
                typename T
            >
        typename std::enable_if< !SQLCMD::is_normal_type<T>::value, void >::type exec()
        {
            executedObject();
            
          //  return (T());
        }

        template
            <
            typename T
            >
        typename std::enable_if< SQLCMD::is_normal_type<T>::value, void >::type exec()
        {

            std::vector< SQLCMD::pair > tmp;
            std::copy(value.begin(), value.end(), std::back_inserter(tmp));
            std::sort(tmp.begin(), tmp.end(), [](SQLCMD::pair lp, SQLCMD::pair rp) {
                return (lp.second.logic < rp.second.logic);
                });
            value.clear();
            std::copy(tmp.begin(), tmp.end(), std::inserter(value, value.begin()));

            if (!CmdExecutor::SELECTABLE_SUPPORT)
            {
                std::vector< SQLCMD::pair > tmp;
                std::copy(value.begin(), value.end(), std::back_inserter(tmp));
                tmp.erase(std::remove_if(tmp.begin(), tmp.end(), [](const SQLCMD::pair& l)
                    {
                        return (!l.second.m_selectable);
                    }), tmp.end());
                value.clear();
                std::copy(tmp.begin(), tmp.end(), std::inserter(value, value.begin()));
                executedObject.setEnd(value.size());
            }
            else
            {
                if (std::find_if(value.begin(), value.end(), [](const SQLCMD::pair& l) {
                    return (l.second.takeAsPredicate != SQLCMD::selector_cmd_type::NONE);
                    }) != value.end())
                {
                    executedObject.setEnd(value.size());
                }
                else
                {
                    executedObject.setEnd(std::count_if(value.begin(), value.end(),
                        [](const SQLCMD::pair& l)
                        {
                          return (l.second.m_selectable);
                        }));
                }
            }

            std::for_each(value.begin(), value.end(), executedObject);
        }



        std::string operator()()
        { //
            exec<dataType>();
            if (0 == limit)
                return (executedObject.get());
            else 
                return (executedObject.get(limit));    
        }
    };

}
#endif