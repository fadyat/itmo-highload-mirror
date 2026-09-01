use proc_macro::TokenStream;
use quote::{quote, quote_spanned};
use syn::{DeriveInput, parse_macro_input, spanned::Spanned};

#[proc_macro_derive(Scan)]
pub fn derive_scan(input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as DeriveInput);

    let scan_body = match input.data {
        syn::Data::Struct(data_struct) => {
            // Generate scan calls for each field, assuming they also implement Scan
            // Assumming for simplicity that all fields are implementing Scan

            let field_scans = data_struct.fields.iter().map(|field| {
                let field_name = &field.ident;
                quote! {
                    self.#field_name.scan(f);
                }
            });

            quote! { #(#field_scans)* }
        }
        _ => {
            let span = input.span();
            return quote_spanned! { span => compile_error!("Scan can only be derived for structs") }.into();
        }
    };

    let name = input.ident;
    let expanded = quote! {
        impl Scan for #name {
            fn scan(&self, f: &mut dyn FnMut(usize)) {
                #scan_body
            }
        }
    };

    TokenStream::from(expanded)
}
