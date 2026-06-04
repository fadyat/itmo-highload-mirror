#![forbid(unsafe_code)]

use std::collections::HashMap;

pub type IniFile = HashMap<String, HashMap<String, String>>;

pub fn parse(content: &str) -> IniFile {
    let mut ini: IniFile = HashMap::new();
    let mut current_section: Option<&str> = None;

    for line in content.lines() {
        let line = line.trim();
        if line.is_empty() {
            continue;
        }

        if line.starts_with('[') && line.ends_with(']') {
            let section = &line[1..line.len() - 1];

            assert!(!section.is_empty(), "Section name cannot be empty");
            assert!(
                !section.contains('[') && !section.contains(']'),
                "Section name cannot contain brackets"
            );

            ini.entry(section.to_string()).or_default();
            current_section = Some(section);
        } else {
            let section = current_section.expect("Key-value pair must be inside a section");

            let parts: Vec<&str> = line.split("=").collect();
            assert!(
                parts.len() <= 2,
                "Key-value pair must contain at most one '='"
            );

            let key = parts[0].trim();
            assert!(!key.is_empty(), "Key cannot be empty");
            let value = parts.get(1).map_or("", |v| v.trim());

            ini.get_mut(section)
                .unwrap()
                .insert(key.to_string(), value.to_string());
        }
    }

    ini
}
