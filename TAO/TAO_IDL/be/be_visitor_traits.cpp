//=============================================================================
/**
*  @file   be_visitor_traits.cpp
*
*  $Id$
*
*  This visitor generates template specializations for traits of various
*  kinds for IDL declarations. These specialized template classes are then
*  used in other template classes in the ORB.
*
*  @author Jeff Parsons <j.parsons@vanderbilt.edu>
*/
//=============================================================================

#include "be_visitor_traits.h"
#include "be_visitor_context.h"
#include "be_root.h"
#include "be_module.h"
#include "be_interface.h"
#include "be_valuetype.h"
#include "be_interface_fwd.h"
#include "be_valuetype_fwd.h"
#include "be_eventtype.h"
#include "be_eventtype_fwd.h"
#include "be_component.h"
#include "be_component_fwd.h"
#include "be_field.h"
#include "be_union_branch.h"
#include "be_exception.h"
#include "be_structure.h"
#include "be_union.h"
#include "be_array.h"
#include "be_typedef.h"
#include "be_helper.h"
#include "be_extern.h"

#include "utl_identifier.h"

ACE_RCSID (be,
           be_visitor_traits,
           "$Id$")

be_visitor_traits::be_visitor_traits (be_visitor_context *ctx)
  : be_visitor_scope (ctx)
{
}

be_visitor_traits::~be_visitor_traits (void)
{
}

int
be_visitor_traits::visit_root (be_root *node)
{
  TAO_OutStream *os = this->ctx_->stream ();

  *os << be_nl << be_nl << "// TAO_IDL - Generated from" << be_nl
      << "// " << __FILE__ << ":" << __LINE__;

  *os << be_nl << be_nl
      << "// Traits specializations." << be_nl
      << "namespace TAO" << be_nl
      << "{" << be_idt;

  if (this->visit_scope (node) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_root - visit scope failed\n"),
                        -1);
    }

  *os << be_uidt_nl
      << "};";

  return 0;
}

int
be_visitor_traits::visit_module (be_module *node)
{
  if (this->visit_scope (node) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_module - visit scope failed\n"),
                        -1);
    }

  return 0;
}

int
be_visitor_traits::visit_interface (be_interface *node)
{
  if (node->cli_traits_gen ())
    {
      return 0;
    }

  TAO_OutStream *os = this->ctx_->stream ();

  // Since the three blocks below generate specialized (i.e., non-template)
  // classes, we don't want to generate them unless it's necessary - thus
  // the logic surrounding each one.

  // I think we need to generate this only for non-defined forward
  // declarations.
  if (!node->imported ())
    {
      os->gen_ifdef_macro (node->flat_name (), "traits");

      *os << be_nl << be_nl
          << "ACE_TEMPLATE_SPECIALIZATION" << be_nl
          << "struct " << be_global->stub_export_macro () << " Objref_Traits<"
          << node->name () << ">" << be_nl
          << "{" << be_idt_nl
          << "static " << node->name () << "_ptr tao_duplicate ("
          << be_idt << be_idt_nl
          << node->name () << "_ptr" << be_uidt_nl
          << ");" << be_uidt_nl
          << "static void tao_release (" << be_idt << be_idt_nl
          << node->name () << "_ptr" << be_uidt_nl
          << ");" << be_uidt_nl
          << "static " << node->name () << "_ptr tao_nil (void);" << be_nl
          << "static CORBA::Boolean tao_marshal (" << be_idt << be_idt_nl
          << node->name () << "_ptr p," << be_nl
          << "TAO_OutputCDR & cdr" << be_uidt_nl
          << ");" << be_uidt << be_uidt_nl
          << "};";

      os->gen_endif ();
    }

  if (this->visit_scope (node) != 0)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_interface - visit scope failed\n"),
                        -1);
    }

  node->cli_traits_gen (I_TRUE);
  return 0;
}

int
be_visitor_traits::visit_interface_fwd (be_interface_fwd *node)
{
  if (node->cli_traits_gen ())
    {
      return 0;
    }

  be_interface *fd =
    be_interface::narrow_from_decl (node->full_definition ());

  // The logic in visit_interface() should handle what gets generated
  // and what doesn't.
  if (this->visit_interface (fd) != 0)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_interface_fwd - code generation failed\n"),
                        -1);
    }

  node->cli_traits_gen (I_TRUE);
  return 0;
}

int
be_visitor_traits::visit_valuetype (be_valuetype *node)
{
  if (node->cli_traits_gen ())
    {
      return 0;
    }

  TAO_OutStream *os = this->ctx_->stream ();

  // I think we need to generate this only for non-defined forward
  // declarations.
  if (!node->imported ())
    {
      os->gen_ifdef_macro (node->flat_name (), "traits");

      *os << be_nl << be_nl
          << "ACE_TEMPLATE_SPECIALIZATION" << be_nl
          << "struct " << be_global->stub_export_macro () << " Value_Traits<"
          << node->name () << ">" << be_nl
          << "{" << be_idt_nl
          << "static void tao_add_ref (" << node->name () << " *);" << be_nl
          << "static void tao_remove_ref (" << node->name () << " *);" 
          << be_nl
          << "static void tao_release (" << node->name () << " *);"
          << be_uidt_nl
          << "};";

      os->gen_endif ();
    }

  int status = this->visit_scope (node);

  if (status != 0)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_valuetype - visit scope failed\n"),
                        -1);
    }

  node->cli_traits_gen (I_TRUE);
  return 0;
}

int
be_visitor_traits::visit_valuetype_fwd (be_valuetype_fwd *node)
{
  if (node->cli_traits_gen ())
    {
      return 0;
    }

  be_valuetype *fd =
    be_valuetype::narrow_from_decl (node->full_definition ());

  // The logic in visit_valuetype() should handle what gets generated
  // and what doesn't.
  int status = this->visit_valuetype (fd);

  if (status != 0)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_valuetype_fwd - code generation failed\n"),
                        -1);
    }

  node->cli_traits_gen (I_TRUE);
  return 0;
}

int
be_visitor_traits::visit_component (be_component *node)
{
  return this->visit_interface (node);
}

int
be_visitor_traits::visit_component_fwd (be_component_fwd *node)
{
  return this->visit_interface_fwd (node);
}

int
be_visitor_traits::visit_eventtype (be_eventtype *node)
{
  return this->visit_valuetype (node);
}

int
be_visitor_traits::visit_eventtype_fwd (be_eventtype_fwd *node)
{
  return this->visit_valuetype_fwd (node);
}

int
be_visitor_traits::visit_field (be_field *node)
{
  be_type *ft = be_type::narrow_from_decl (node->field_type ());
  AST_Decl::NodeType nt = ft->node_type ();
  
  // All we are trying to catch in here are anonymous array members.
  if (nt != AST_Decl::NT_array)
    {
      return 0;
    }

  if (ft->accept (this) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_field - visit field type failed\n"),
                        -1);
    }

  return 0;
}

int
be_visitor_traits::visit_union_branch (be_union_branch *node)
{
  be_type *ft = be_type::narrow_from_decl (node->field_type ());
  AST_Decl::NodeType nt = ft->node_type ();
  
  // All we are trying to catch in here are anonymous array members.
  if (nt != AST_Decl::NT_array)
    {
      return 0;
    }

  if (ft->accept (this) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_union_branch - visit field type failed\n"),
                        -1);
    }

  return 0;
}

int
be_visitor_traits::visit_exception (be_exception *node)
{
  if (this->visit_scope (node) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_exception - visit scope failed\n"),
                        -1);
    }

  return 0;
}

int
be_visitor_traits::visit_struct (be_structure *node)
{
  if (this->visit_scope (node) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_struct - visit scope failed\n"),
                        -1);
    }

  return 0;
}

int
be_visitor_traits::visit_union (be_union *node)
{
  if (this->visit_scope (node) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_union - visit scope failed\n"),
                        -1);
    }

  return 0;
}

int
be_visitor_traits::visit_array (be_array *node)
{
  if (node->imported () || node->cli_traits_gen ())
    {
      return 0;
    }
    
  ACE_CString name_holder;
  
  if (node->is_nested ())
    {
      be_decl *parent = 
        be_scope::narrow_from_scope (node->defined_in ())->decl ();
      name_holder = parent->full_name ();
      
      name_holder += "::";
      
      if (!this->ctx_->alias ())
        {
          name_holder += "_";
        }
        
      name_holder += node->local_name ()->get_string ();
    }
  else
    {
      name_holder = node->full_name ();
    }
  
  const char *name = name_holder.fast_rep ();
    
  TAO_OutStream *os = this->ctx_->stream ();

  *os << be_nl << be_nl
      << "ACE_TEMPLATE_SPECIALIZATION" << be_nl
      << "struct " << be_global->stub_export_macro () << " Array_Traits<"
      << name << ", " << name
      << "_slice>" << be_nl
      << "{" << be_idt_nl
      << "static void tao_free (" << be_idt << be_idt_nl
      << name << "_slice * _tao_slice" << be_uidt_nl
      << ");" << be_uidt_nl
      << "static " << name << "_slice * tao_dup (" 
      << be_idt << be_idt_nl
      << "const " << name << "_slice * _tao_slice" 
      << be_uidt_nl
      << ");" << be_uidt_nl
      << "static void tao_copy (" << be_idt << be_idt_nl
      << name << "_slice * _tao_to," << be_nl
      << "const " << name << "_slice * _tao_from" 
      << be_uidt_nl
      << ");" << be_uidt_nl
      << "static " << name << "_slice * tao_alloc (void);" 
      << be_uidt_nl
      << "};";

  node->cli_traits_gen (I_TRUE);
  return 0;
}

int
be_visitor_traits::visit_typedef (be_typedef *node)
{
  this->ctx_->alias (node);

  // Make a decision based on the primitive base type.
  be_type *bt = node->primitive_base_type ();

  if (!bt || (bt->accept (this) == -1))
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_traits::"
                         "visit_typedef - "
                         "Bad primitive type\n"),
                        -1);
    }

  this->ctx_->alias (0);
  node->cli_traits_gen (I_TRUE);
  return 0;
}
