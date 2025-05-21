import sqlite3
import pandas as pd

# Caminho para seu arquivo SQLite no Google Drive (ajuste se necessário)
caminho_db = r'C:\Users\Eve\Desktop\Repositorio ETD\SoccerPlayerAttributes\database.sqlite'

# Conecta ao banco de dados
conn = sqlite3.connect(caminho_db)

# Lê a tabela 'Player_Attributes' e salva no DataFrame df
df = pd.read_sql_query("SELECT * FROM Player_Attributes", conn)

# Exibe os primeiros registros pra confirmar
print(df)
