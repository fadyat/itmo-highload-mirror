use crate::{
    ObjectId,
    data::{DataType, Value},
    error::{Error, MissingColumnError, NotFoundError, Result, UnexpectedTypeError},
    object::{Schema, SchemaField},
};

use rusqlite::{ToSql, params};

use std::borrow::Cow;

////////////////////////////////////////////////////////////////////////////////

pub type Row<'a> = Vec<Value<'a>>;
pub type RowSlice<'a> = [Value<'a>];

////////////////////////////////////////////////////////////////////////////////

pub(crate) trait StorageTransaction {
    fn table_exists(&self, table: &str) -> Result<bool>;
    fn create_table(&self, schema: &Schema) -> Result<()>;
    fn insert_row(&self, schema: &Schema, row: &RowSlice) -> Result<ObjectId>;
    fn update_row(&self, id: ObjectId, schema: &Schema, row: &RowSlice) -> Result<()>;
    fn select_row(&self, id: ObjectId, schema: &Schema) -> Result<Row<'static>>;
    fn delete_row(&self, id: ObjectId, schema: &Schema) -> Result<()>;
    fn commit(&self) -> Result<()>;
    fn rollback(&self) -> Result<()>;
}

impl<'a> StorageTransaction for rusqlite::Transaction<'a> {
    fn table_exists(&self, table: &str) -> Result<bool> {
        let count: i64 = self.query_row(
            "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = ?",
            [table],
            |row| row.get(0),
        )?;
        Ok(count > 0)
    }

    fn create_table(&self, schema: &Schema) -> Result<()> {
        let cols = schema
            .fields
            .iter()
            .map(SchemaField::to_sql)
            .collect::<String>();
        let sql = format!(
            "CREATE TABLE {} ( id INTEGER PRIMARY KEY AUTOINCREMENT{} )",
            schema.table_name, cols
        );
        self.execute(&sql, [])?;
        Ok(())
    }

    fn insert_row(&self, schema: &Schema, row: &RowSlice) -> Result<ObjectId> {
        let res = if schema.fields.is_empty() {
            self.execute(
                &format!("INSERT INTO {} DEFAULT VALUES", schema.table_name),
                [],
            )
        } else {
            let placeholders = (1..=schema.fields.len())
                .map(|i| format!("?{}", i))
                .collect::<Vec<_>>()
                .join(", ");
            let sql = format!(
                "INSERT INTO {}({}) VALUES ({})",
                schema.table_name,
                schema.get_column_names(),
                placeholders
            );
            let params_vec: Vec<&dyn ToSql> = row.iter().map(|x| x as &dyn ToSql).collect();
            self.execute(&sql, &*params_vec)
        };

        match res {
            Ok(_) => Ok(self.last_insert_rowid().into()),
            Err(error) => {
                let prefix = format!("table {} has no column named ", schema.table_name);
                let err_str = error.to_string();
                if err_str.starts_with(&prefix) {
                    let column = err_str[prefix.len()..].to_string();
                    let c = schema.get_field(&column);
                    return Err(Error::MissingColumn(Box::new(MissingColumnError {
                        type_name: schema.type_name,
                        attr_name: c.field_name,
                        table_name: schema.table_name,
                        column_name: c.column_name,
                    })));
                }
                Err(error.into())
            }
        }
    }

    fn update_row(&self, id: ObjectId, schema: &Schema, row: &RowSlice) -> Result<()> {
        let set_clause = schema
            .fields
            .iter()
            .enumerate()
            .map(|(i, sf)| format!("{} = ?{}", sf.column_name, i + 1))
            .collect::<Vec<_>>()
            .join(", ");
        let sql = format!(
            "UPDATE {} SET {} WHERE id = {}",
            schema.table_name,
            set_clause,
            Into::<i64>::into(id)
        );
        let params_vec: Vec<&dyn ToSql> = row.iter().map(|x| x as &dyn ToSql).collect();
        self.execute(&sql, &*params_vec)?;
        Ok(())
    }

    fn select_row(&self, id: ObjectId, schema: &Schema) -> Result<Row<'static>> {
        let cols = if schema.fields.is_empty() {
            "*".to_string()
        } else {
            schema.get_column_names()
        };
        let sql = format!("SELECT {} FROM {} WHERE id = (?1)", cols, schema.table_name);

        match self.prepare(&sql) {
            Ok(mut stmt) => {
                let row_res = stmt.query_row(params![Into::<i64>::into(id)], |r| {
                    schema
                        .fields
                        .iter()
                        .enumerate()
                        .map(|(i, sf)| {
                            Ok(match sf.data_type {
                                DataType::String => Value::String(Cow::Owned(r.get(i)?)),
                                DataType::Bytes => Value::Bytes(Cow::Owned(r.get(i)?)),
                                DataType::Int64 => Value::Int64(r.get(i)?),
                                DataType::Float64 => Value::Float64(r.get(i)?),
                                DataType::Bool => Value::Bool(r.get(i)?),
                            })
                        })
                        .collect::<std::result::Result<Vec<Value>, rusqlite::Error>>()
                });

                match row_res {
                    Ok(values) => Ok(values),
                    Err(rusqlite::Error::QueryReturnedNoRows) => {
                        Err(Error::NotFound(Box::new(NotFoundError {
                            object_id: id,
                            type_name: schema.type_name,
                        })))
                    }
                    Err(rusqlite::Error::InvalidColumnType(_, col_name, got)) => {
                        let sf = schema.get_field(&col_name);
                        Err(Error::UnexpectedType(Box::new(UnexpectedTypeError {
                            type_name: schema.type_name,
                            attr_name: sf.field_name,
                            table_name: schema.table_name,
                            column_name: sf.column_name,
                            expected_type: sf.data_type,
                            got_type: got.to_string(),
                        })))
                    }
                    Err(e) => Err(e.into()),
                }
            }

            Err(e) => {
                let s = e.to_string();
                if let Some(column) = s.strip_prefix("no such column: ") {
                    let c = schema.get_field(column);
                    return Err(Error::MissingColumn(Box::new(MissingColumnError {
                        type_name: schema.type_name,
                        attr_name: c.field_name,
                        table_name: schema.table_name,
                        column_name: c.column_name,
                    })));
                }
                Err(e.into())
            }
        }
    }

    fn delete_row(&self, id: ObjectId, schema: &Schema) -> Result<()> {
        self.execute(
            &format!("DELETE FROM {} WHERE id = ?", schema.table_name),
            params![Into::<i64>::into(id)],
        )?;
        Ok(())
    }

    fn commit(&self) -> Result<()> {
        self.execute("COMMIT", [])?;
        Ok(())
    }

    fn rollback(&self) -> Result<()> {
        self.execute("ROLLBACK", [])?;
        Ok(())
    }
}
