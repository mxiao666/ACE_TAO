// $Id$

#ifndef CIAO_UPGRADEABLE_CONTEXT_IMPL_T_C
#define CIAO_UPGRADEABLE_CONTEXT_IMPL_T_C

#include "Upgradeable_Context_Impl_T.h"

namespace CIAO
{
  template <typename BASE_CTX, 
            typename SVNT, 
            typename COMP, 
            typename COMP_VAR>
  Upgradeable_Context_Impl<BASE_CTX, SVNT, COMP, COMP_VAR>::
      Upgradeable_Context_Impl (
              Components::CCMHome_ptr home,
              Session_Container *c,
              SVNT *sv)
    : Context_Impl<BASE_CTX, SVNT, COMP, COMP_VAR> (home, c, sv),
      servant_ (sv)
  {
  }

  template <typename BASE_CTX, 
            typename SVNT, 
            typename COMP, 
            typename COMP_VAR>
  Upgradeable_Context_Impl<BASE_CTX, SVNT, COMP, COMP_VAR>::
       ~Upgradeable_Context_Impl (void)
  {
  }

  template <typename BASE_CTX, 
            typename SVNT, 
            typename COMP, 
            typename COMP_VAR>
  void
  Upgradeable_Context_Impl<BASE_CTX, SVNT, COMP, COMP_VAR>::deactivate_facet (
      const char* obj_id
      ACE_ENV_SINGLE_ARG_DECL
    )
    ACE_THROW_SPEC ((CORBA::SystemException))
  {
    PortableServer::ObjectId_var oid =
        PortableServer::string_to_ObjectId (obj_id);

    this->container_->deactivate_facet (oid);
  }

  template <typename BASE_CTX, 
            typename SVNT, 
            typename COMP, 
            typename COMP_VAR>
  void
  Upgradeable_Context_Impl<BASE_CTX, SVNT, COMP, COMP_VAR>::
     update_port_activator (
           const char* obj_id
           ACE_ENV_SINGLE_ARG_DECL)
    ACE_THROW_SPEC ((CORBA::SystemException))
  {
    PortableServer::ObjectId_var oid =
        PortableServer::string_to_ObjectId (obj_id);
    CIAO::Servant_Activator *sa =
        this->container_->ports_servant_activator ();
    sa->update_port_activator (oid);
  }

}

#endif /* CIAO_UPGRADEABLE_CONTEXT_IMPL_T_C */
