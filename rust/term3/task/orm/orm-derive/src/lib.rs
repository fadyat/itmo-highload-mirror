use proc_macro::TokenStream;
use quote::quote;
use syn::{
    Attribute, Data::Struct, DataStruct, DeriveInput, Fields::Named, FieldsNamed, Ident, Lit::Str,
    Meta::List, MetaList, NestedMeta::Lit, parse_macro_input,
};

#[proc_macro_derive(Object, attributes(table_name, column_name))]
pub fn derive_object(stream: TokenStream) -> TokenStream {
    let DeriveInput {
        ident: struct_ident,
        data,
        generics,
        attrs,
        ..
    }: DeriveInput = parse_macro_input!(stream);

    let (impl_gen, ty_gen, where_clause) = generics.split_for_impl();
    let type_name = struct_ident.to_string();
    let table_name = get_attribute("table_name", &attrs, &type_name);

    let Struct(DataStruct { fields, .. }) = data else {
        return quote!(compile_error!("#[derive(Object)] only supports structs")).into();
    };
    let mut field_idents: Vec<Ident> = Vec::new();
    let schema_fields = if let Named(FieldsNamed { named, .. }) = fields {
        named
            .into_iter()
            .map(|field| {
                let field_ident = field.ident.unwrap();
                let field_name = field_ident.to_string();
                let column_name = get_attribute("column_name", &field.attrs, &field_name);
                let field_datatype = match get_type(&field.ty) {
                    Ok(id) => id,
                    Err(msg) => return quote!(compile_error!(#msg)),
                };
                field_idents.push(field_ident);

                quote![
                    orm::object::SchemaField {
                        field_name: #field_name,
                        column_name: #column_name,
                        data_type: orm::data::DataType::#field_datatype,
                    }
                ]
            })
            .collect()
    } else {
        vec![]
    };

    let tokens = quote! {
        impl #impl_gen Object for #struct_ident #ty_gen
        #where_clause
        {
            const SCHEMA: orm::object::Schema = orm::object::Schema {
                type_name: #type_name,
                table_name: #table_name,
                fields: &[#(#schema_fields),*],
            };
            fn to_row<'s>(&'s self) -> orm::storage::Row<'s> {
                vec![#(orm::data::Value::from(&self.#field_idents)),*]
            }
            fn from_row<'s>(row: &'s orm::storage::RowSlice<'s>) -> Self {
                let mut row_iter = row.iter();
                Self {
                    #(#field_idents: row_iter.next().unwrap().into(),)*
                }
            }
        }
    };
    tokens.into()
}

fn get_attribute(ident: &str, attrs: &[Attribute], default: &str) -> String {
    let mut found: Option<String> = None;
    for attr in attrs {
        match attr.parse_meta() {
            Ok(List(MetaList { path, nested, .. })) if path.is_ident(ident) => {
                if let Some(Lit(Str(name))) = nested.first() {
                    let val = name.value();
                    if found.is_some() {
                        panic!("multiple {} calls", ident);
                    }
                    found = Some(val);
                }
            }
            _ => {}
        }
    }
    found.unwrap_or_else(|| default.to_owned())
}

fn get_type(ty: &syn::Type) -> Result<Ident, String> {
    use syn::{GenericArgument, PathArguments, Type, TypePath};

    // collapse nested if-let by extracting the last path segment (if any)
    let seg_opt = match ty {
        Type::Path(TypePath { path, .. }) => path.segments.last(),
        _ => None,
    };

    if let Some(seg) = seg_opt {
        let name = seg.ident.to_string();
        let span = seg.ident.span();
        match name.as_str() {
            "String" => return Ok(Ident::new("String", span)),
            "i64" => return Ok(Ident::new("Int64", span)),
            "f64" => return Ok(Ident::new("Float64", span)),
            "bool" => return Ok(Ident::new("Bool", span)),
            "Vec" => {
                let is_u8 = matches!(
                    &seg.arguments,
                    PathArguments::AngleBracketed(args)
                    if args.args.first().and_then(|g| {
                        if let GenericArgument::Type(Type::Path(tp)) = g {
                            tp.path.segments.last().map(|s| s.ident == "u8")
                        } else { None }
                    }).unwrap_or(false)
                );
                if is_u8 {
                    return Ok(Ident::new("Bytes", span));
                }
            }
            _ => {}
        }
    }

    Err(format!("unsupported field type: {}", quote::quote!(#ty)))
}
