import sqlite3
import pandas as pd

# Caminho para seu arquivo SQLite no Google Drive (ajuste se necessário)
caminho_db = r'C:\Users\Eve\Desktop\Repositorio ETD\SoccerPlayerAttributes\database.sqlite'

# Conecta ao banco de dados
conn = sqlite3.connect(caminho_db)

# Lê a tabela 'Player_Attributes' e salva no DataFrame df
df = pd.read_sql_query("SELECT * FROM Player_Attributes", conn)

# List of columns to be dropped
columns_to_drop = ['id', 'player_fifa_api_id', 'player_api_id', 'date',
                   'gk_diving', 'gk_handling', 'gk_kicking', 'gk_positioning', 'gk_reflexes']

# Drop the specified columns
df = df.drop(columns=columns_to_drop, errors='ignore')


# Ajustar para mostrar todas as colunas
pd.set_option('display.max_columns', None)

# Mostrar os 5 primeiros e os 5 últimos registros com todas as colunas
print(df)

# Salvar o dataframe como CSV (sem índice extra)
df.to_csv('C:/Users/Eve/Desktop/Repositorio ETD/SoccerPlayerAttributes/dataset_limpo.csv', index=False)

print("Arquivo CSV salvo com sucesso!")


