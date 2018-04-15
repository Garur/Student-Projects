using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine
{
    /// <summary>
    /// Provides access to engine services. The services are modules of the engine 
    /// and must implement the IProvidable interface
    /// The service provider is a singleton, this makes every service accessible 
    /// from any place in the application.
    /// The service provider keeps a Dictionary of services and the services are
    /// identified by their service interface. 
    /// </summary>
    class ServiceLocator
    {
        private static ServiceLocator instance;
        public static ServiceLocator Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new ServiceLocator();
                }
                return instance;
            }
        }
        //Dictionary Containing all the services
        //Services must be IProvidable and implement another interface
        //The second interface will be the key to identify the service
        IDictionary<Type, IProvidable> services;

        private ServiceLocator()
        {
            services = new Dictionary<Type, IProvidable>();
        }

        /// <summary>
        /// Instanciaties a new service of type T. Service must be a IProvidable
        /// The service will be identified by the interface that it implements apart from the IProvidable
        /// The service is store in the services dictionary and the key will be the interface type 
        /// </summary>
        /// <typeparam name="T">Type of the service</typeparam>
        /// <returns>Returns the service instance that as been instanciated</returns>
        public T StartService<T>() where T : IProvidable, new()
        {
            Type interfaceType = null;

            Type[] array = typeof(T).GetInterfaces();
            //Retrives the interface type that is not the IProvidable
            for (int i = 0; i < array.Length; i++)
            {
                if(array[i] != typeof(IProvidable))
                {
                    interfaceType = array[i];
                    break;
                }
            }
            //Checks if the service is valid
            if (interfaceType == null)
            {
                throw new NullReferenceException("InterfaceType is Null");
            }

            //Check if service is already active
            if (services.ContainsKey(interfaceType))
            {
                Console.WriteLine("Service already active");
                return default(T);
            }

            //Instanciates the new service
            T s = new T();
            //Adds the service and its type interface to the dictionary
            services.Add(interfaceType,s);
            return s;
        }

        /// <summary>
        /// Returns a service of type T 
        /// If the service is not found returns null
        /// </summary>
        /// <typeparam name="T">Type of the service</typeparam>
        /// <returns>Returns the service of type IProvidable if found</returns>
        public T GetService<T>() where T : IProvidable
        {
            Type t = typeof(T);
            if (services.ContainsKey(t))
            {
                return (T)services[t];
            }
            return default(T);            
        }
    }
}
