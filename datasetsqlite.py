import sqlite3
import pandas as pd

# Caminho para seu arquivo SQLite no Google Drive (ajuste se necessário)
caminho_db = 'C:/Users/55119/Desktop/Facul/Atividades/3Sem/Estrutura de Dados/Trabalho/SoccerPlayerAttributes/database.sqlite'

# Conecta ao banco de dados
conn = sqlite3.connect(caminho_db)

# Lê a tabela 'Player_Attributes' e salva no DataFrame df
df = pd.read_sql_query("SELECT * FROM Player_Attributes", conn)

# Exibe os primeiros registros pra confirmar
print(df)

# Exibe o nome das colunas
print(list(df.columns))