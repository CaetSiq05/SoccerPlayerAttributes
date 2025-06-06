import sqlite3
import pandas as pd

# Caminho para seu arquivo SQLite
caminho_db = r'C:\Users\Eve\Desktop\Repositorio ETD\SoccerPlayerAttributes\database.sqlite'

# Conecta ao banco de dados
conn = sqlite3.connect(caminho_db)

# Lê a tabela 'Player_Attributes' e salva no DataFrame
df = pd.read_sql_query("SELECT * FROM Player_Attributes", conn)

# Remove linhas com valores ausentes
df.dropna(axis=0, inplace=True)

# Remove duplicatas com base em 'player_fifa_api_id', mantendo o menor 'id'
df = df.sort_values('id').drop_duplicates(subset='player_fifa_api_id', keep='first')

# Remove colunas desnecessárias
columns_to_drop = ['id', 'player_api_id', 'date',
                   'gk_diving', 'gk_handling', 'gk_kicking', 'gk_positioning', 'gk_reflexes']
df = df.drop(columns=columns_to_drop, errors='ignore')

# Mostra todas as colunas no print
pd.set_option('display.max_columns', None)

# Mostra os 5 primeiros e os 5 últimos registros
print(df)

# Salva o DataFrame como CSV
df.to_csv('C:/Users/Eve/Desktop/Repositorio ETD/SoccerPlayerAttributes/dataset_limpo3.csv', index=False)

print("✅ Arquivo CSV salvo com sucesso sem duplicatas de ID!")
