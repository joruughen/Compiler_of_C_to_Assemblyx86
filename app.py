import streamlit as st
import subprocess
import tempfile
import os
import re
import requests
import json
import platform

# Configuración de la página
st.set_page_config(
    page_title="🧪 C to Assembly Compiler",
    page_icon="🧪",
    layout="wide"
)

# CSS mejorado con diseño más moderno
st.markdown("""
<style>
    @import url('https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@400;600;700&display=swap');
    
    .main > div {
        padding-top: 2rem;
    }
    
    .main-header {
        background: linear-gradient(135deg, #0f3460 0%, #16537e 50%, #0f3460 100%);
        border: 3px solid #39ff14;
        padding: 2rem;
        border-radius: 20px;
        text-align: center;
        color: #39ff14;
        font-family: 'Space Grotesk', sans-serif;
        font-size: 3rem;
        font-weight: 700;
        margin-bottom: 3rem;
        text-shadow: 0 0 20px rgba(57, 255, 20, 0.5);
        box-shadow: 0 10px 30px rgba(15, 52, 96, 0.3);
        position: relative;
        overflow: hidden;
    }
    
    .main-header::before {
        content: '';
        position: absolute;
        top: -50%;
        left: -50%;
        width: 200%;
        height: 200%;
        background: linear-gradient(45deg, transparent, rgba(57, 255, 20, 0.1), transparent);
        animation: shimmer 3s infinite;
    }
    
    @keyframes shimmer {
        0% { transform: translateX(-100%) translateY(-100%) rotate(45deg); }
        100% { transform: translateX(100%) translateY(100%) rotate(45deg); }
    }
    
    .code-container {
        background: linear-gradient(145deg, #1a1a2e 0%, #16213e 100%);
        border: 2px solid #00ff41;
        border-radius: 15px;
        padding: 1.5rem;
        margin: 1rem 0;
        box-shadow: 
            0 8px 32px rgba(0, 255, 65, 0.1),
            inset 0 1px 0 rgba(255, 255, 255, 0.1);
        backdrop-filter: blur(10px);
    }
    
    .code-header {
        color: #00ff41;
        font-family: 'Space Grotesk', sans-serif;
        font-size: 1.5rem;
        font-weight: 600;
        margin-bottom: 1rem;
        display: flex;
        align-items: center;
        gap: 0.5rem;
    }
    
    .assembly-output {
        background: #0a0a0a;
        border: 2px solid #39ff14;
        border-radius: 12px;
        padding: 1.5rem;
        font-family: 'Courier New', monospace !important;
        color: #39ff14;
        font-size: 14px;
        line-height: 1.4;
        max-height: 600px;
        overflow-y: auto;
        box-shadow: 
            0 0 20px rgba(57, 255, 20, 0.2),
            inset 0 0 20px rgba(0, 0, 0, 0.5);
        position: relative;
        white-space: pre-wrap;
        word-wrap: break-word;
    }
    
    .compile-btn {
        background: linear-gradient(45deg, #00ff41, #39ff14) !important;
        border: none !important;
        border-radius: 25px !important;
        padding: 0.8rem 2rem !important;
        font-family: 'Space Grotesk', sans-serif !important;
        font-weight: 600 !important;
        font-size: 1.1rem !important;
        color: #0a0a0a !important;
        text-transform: uppercase !important;
        letter-spacing: 1px !important;
        cursor: pointer !important;
        transition: all 0.3s ease !important;
        box-shadow: 0 4px 15px rgba(0, 255, 65, 0.4) !important;
        width: 100% !important;
        margin: 1rem 0 !important;
    }
    
    .compile-btn:hover {
        transform: translateY(-2px) !important;
        box-shadow: 0 6px 20px rgba(0, 255, 65, 0.6) !important;
        filter: brightness(1.1) !important;
    }
    
    .stats-container {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
        gap: 1rem;
        margin: 2rem 0;
    }
    
    .stat-card {
        background: linear-gradient(135deg, #1e3a8a 0%, #3730a3 100%);
        border: 1px solid #60a5fa;
        border-radius: 12px;
        padding: 1.5rem;
        text-align: center;
        color: white;
    }
    
    .stat-number {
        font-size: 2rem;
        font-weight: 700;
        color: #60a5fa;
        font-family: 'Courier New', monospace;
    }
    
    .stTextArea textarea {
        background-color: #0a0a0a !important;
        color: #39ff14 !important;
        border: 2px solid #00ff41 !important;
        border-radius: 10px !important;
        font-family: 'Courier New', monospace !important;
        font-size: 14px !important;
        line-height: 1.4 !important;
        tab-size: 4 !important;
        -moz-tab-size: 4 !important;
    }
    
    .stTextArea textarea:focus {
        border-color: #39ff14 !important;
        box-shadow: 0 0 15px rgba(57, 255, 20, 0.3) !important;
    }
</style>
""", unsafe_allow_html=True)

# Header principal mejorado
st.markdown("""
<div class="main-header">
    🧪 C TO ASSEMBLY COMPILER 🧪
    <div style="font-size: 1rem; margin-top: 0.5rem; opacity: 0.8;">
        Compiler_of_C_to_Assemblyx86 Project Interface
    </div>
</div>
""", unsafe_allow_html=True)

def check_gcc_installation():
    """Verifica si GCC está instalado y accesible"""
    try:
        result = subprocess.run(['gcc', '--version'], capture_output=True, text=True, timeout=5)
        return result.returncode == 0
    except:
        return False

def compile_and_run_asm_local(asm_code):
    """
    Compila y ejecuta código assembly localmente usando GCC
    """
    try:
        # Crear archivo temporal para el código assembly
        with tempfile.NamedTemporaryFile(mode='w', suffix='.s', delete=False) as temp_asm:
            temp_asm.write(asm_code)
            temp_asm_name = temp_asm.name

        # Nombre del archivo ejecutable
        exec_name = 'asm_program.exe' if platform.system() == 'Windows' else 'asm_program'

        try:
            # Compilar el código assembly
            compile_cmd = ['gcc', '-no-pie', temp_asm_name, '-o', exec_name]
            if platform.system() == 'Windows':
                compile_cmd = ['gcc', temp_asm_name, '-o', exec_name]

            compile_result = subprocess.run(
                compile_cmd,
                capture_output=True,
                text=True,
                timeout=10,
                cwd=tempfile.gettempdir()
            )

            if compile_result.returncode != 0:
                return None, f"❌ Error de compilación GCC:\n{compile_result.stderr}"

            # Ejecutar el programa compilado
            exec_path = os.path.join(tempfile.gettempdir(), exec_name)
            run_result = subprocess.run(
                [exec_path],
                capture_output=True,
                text=True,
                timeout=5,
                cwd=tempfile.gettempdir()
            )

            return {
                'stdout': run_result.stdout,
                'stderr': run_result.stderr,
                'returncode': run_result.returncode
            }, None

        finally:
            # Limpiar archivos temporales
            try:
                os.unlink(temp_asm_name)
                exec_path = os.path.join(tempfile.gettempdir(), exec_name)
                if os.path.exists(exec_path):
                    os.unlink(exec_path)
            except:
                pass

    except Exception as e:
        return None, f"❌ Error: {str(e)}"

def run_compiler_executable(c_code, compiler_path="./compiler"):
    """
    Ejecuta el compilador real del proyecto Compiler_of_C_to_Assemblyx86
    """
    try:
        if not os.path.exists(compiler_path):
            return None, f"No se encontró el ejecutable del compilador en: {compiler_path}\n\nPasos:\n1. Compila: g++ *.cpp -o compiler\n2. Verifica la ruta del ejecutable"

        if not os.access(compiler_path, os.X_OK):
            return None, f"Sin permisos de ejecución: chmod +x {compiler_path}"

        # Crear archivo temporal con el código C
        with tempfile.NamedTemporaryFile(mode='w', suffix='.c', delete=False) as temp_c:
            temp_c.write(c_code)
            temp_c_path = temp_c.name

        # Ejecutar el compilador
        result = subprocess.run(
            [compiler_path, temp_c_path],
            capture_output=True,
            text=True,
            timeout=30,
            cwd=os.path.dirname(os.path.abspath(compiler_path)) if os.path.dirname(compiler_path) else None
        )

        os.unlink(temp_c_path)

        if result.returncode != 0:
            return None, f"Error de compilación:\n{result.stderr}"

        # Extraer assembly del output
        output_lines = result.stdout.split('\n')
        assembly_start = -1
        for i, line in enumerate(output_lines):
            if "Run program:" in line:
                assembly_start = i + 1
                break

        if assembly_start == -1:
            return None, f"No se encontró 'Run program:' en la salida"

        assembly_lines = []
        for i in range(assembly_start, len(output_lines)):
            line = output_lines[i]
            if "End of program execution" in line:
                break
            if line.strip():
                assembly_lines.append(line)

        assembly_code = '\n'.join(assembly_lines)
        if not assembly_code.strip():
            return None, "No se generó código assembly"

        return assembly_code, None

    except subprocess.TimeoutExpired:
        return None, "Timeout: El compilador tardó demasiado (>30s)"
    except Exception as e:
        return None, f"Error: {str(e)}"

def execute_c_code_jdoodle(c_code, client_id, client_secret):
    """Ejecuta código C usando la API de JDoodle"""
    url = "https://api.jdoodle.com/v1/execute"

    payload = {
        "clientId": client_id,
        "clientSecret": client_secret,
        "script": c_code,
        "language": "c",
        "versionIndex": "0",
        "stdin": ""
    }

    headers = {"Content-Type": "application/json"}

    try:
        response = requests.post(url, headers=headers, data=json.dumps(payload), timeout=30)
        result = response.json()

        if response.status_code == 200:
            return result, None
        else:
            return None, f"Error: {result}"

    except Exception as e:
        return None, f"Request error: {str(e)}"

# Configuración de JDoodle API (solo para C)
with st.expander("🔑 JDoodle API Settings (for C execution)", expanded=False):
    client_id = st.text_input("Client ID", value="2829c888bcef3aca818d0a9aa7ba5679")
    client_secret = st.text_input("Client Secret", value="f6991d6f133bc9a962819c8877e044627a18b9e774d2103fb412d1ef913f7c87", type="password")
    st.info("💡 Get free credentials at: [JDoodle.com](https://www.jdoodle.com)")

# Layout principal
col1, col2 = st.columns([1, 1], gap="large")

with col1:
    st.markdown("""
    <div class="code-container">
        <div class="code-header">
            🔬 C Source Code Laboratory
        </div>
    </div>
    """, unsafe_allow_html=True)

    c_code = st.text_area(
        "",
        value="""#include <stdio.h>
        
int main() {
    int x;
    int y;
    int result;
    x = 42;
    y = 13;
    result = x + y;
    printf("Result: %d", result);
    return 0;
}""",
        height=300,
        label_visibility="collapsed",
        key="c_code_editor"
    )

    # Botones de acción para C
    col_compile, col_run_c = st.columns(2)

    with col_compile:
        compile_clicked = st.button("🔧 COMPILE TO ASSEMBLY", key="compile_btn", use_container_width=True)

    with col_run_c:
        run_c_clicked = st.button("▶️ RUN C CODE", key="run_c_btn", use_container_width=True)

    # Procesar compilación
    if compile_clicked and c_code.strip():
        with st.spinner("⚡ Running real compiler pipeline..."):
            assembly_result, error = run_compiler_executable(c_code, "./compiler")

            if error:
                st.error(f"❌ Compilation Error: {error}")
            else:
                st.session_state['assembly_output'] = assembly_result
                st.success("✅ Assembly generated!")

    # Procesar ejecución de código C
    if run_c_clicked:
        if not client_id or not client_secret:
            st.error("❌ Please enter your JDoodle API credentials!")
        elif not c_code.strip():
            st.error("❌ Please write some C code first!")
        else:
            with st.spinner("🔥 Executing C code..."):
                result, error = execute_c_code_jdoodle(c_code, client_id, client_secret)

                if error:
                    st.error(f"❌ C Execution Error: {error}")
                elif result:
                    st.session_state['c_execution_output'] = result
                    if result.get('statusCode') == 200:
                        st.success("✅ C code executed successfully!")
                    else:
                        st.warning(f"⚠️ C execution status: {result.get('statusCode', 'Unknown')}")

    # Mostrar output de C
    if 'c_execution_output' in st.session_state:
        st.markdown("### 📊 C Code Execution Results")
        result = st.session_state['c_execution_output']

        if 'output' in result and result['output'] and result['output'].strip():
            st.markdown("**Program Output:**")
            st.code(result['output'], language="text")

        col_stats1, col_stats2, col_stats3 = st.columns(3)
        with col_stats1:
            status = result.get('statusCode', 'N/A')
            if status == 200:
                st.success(f"✅ Status: {status}")
            else:
                st.error(f"❌ Status: {status}")
        with col_stats2:
            st.info(f"💾 Memory: {result.get('memory', 'N/A')} KB")
        with col_stats3:
            st.info(f"⏱️ CPU: {result.get('cpuTime', 'N/A')}s")

        if 'error' in result and result['error'] and result['error'].strip():
            st.error(f"**Errors:** {result['error']}")

with col2:
    st.markdown("""
    <div class="code-container">
        <div class="code-header">
            ⚡ x86-64 Assembly Output Portal
        </div>
    </div>
    """, unsafe_allow_html=True)

    if 'assembly_output' in st.session_state:
        st.text_area(
            "Generated Assembly:",
            value=st.session_state['assembly_output'],
            height=300,
            label_visibility="collapsed",
            key="assembly_display"
        )

        # Ejecutar assembly localmente si GCC está disponible
        gcc_available = check_gcc_installation()

        if gcc_available:
            execute_clicked = st.button("🚀 EXECUTE ASSEMBLY (Local GCC)", key="execute_btn", use_container_width=True)

            if execute_clicked:
                with st.spinner("🔥 Executing assembly with local GCC..."):
                    result, error = compile_and_run_asm_local(st.session_state['assembly_output'])

                    if error:
                        st.error(error)
                    else:
                        st.session_state['assembly_execution_output'] = result
                        if result['returncode'] == 0:
                            st.success("✅ Assembly executed successfully!")
                        else:
                            st.warning(f"⚠️ Assembly exit code: {result['returncode']}")
        else:
            st.warning("⚠️ GCC not available - install GCC to execute assembly locally")

        st.download_button(
            label="📁 Download Assembly File",
            data=st.session_state['assembly_output'],
            file_name="output.s",
            mime="text/plain",
            use_container_width=True
        )
    else:
        st.markdown("""
        <div class="assembly-output">
Real assembly output will appear here after compilation...

Uses the actual Compiler_of_C_to_Assemblyx86 project! 🚀
        </div>
        """, unsafe_allow_html=True)

    # Mostrar resultados de assembly
    if 'assembly_execution_output' in st.session_state:
        st.markdown("### 📊 Assembly Execution Results")
        result = st.session_state['assembly_execution_output']

        if result['stdout'] and result['stdout'].strip():
            st.markdown("**Program Output:**")
            st.code(result['stdout'], language="text")

        if result['stderr'] and result['stderr'].strip():
            st.error(f"**Errors:** {result['stderr']}")

        col_stats1, col_stats2 = st.columns(2)
        with col_stats1:
            if result['returncode'] == 0:
                st.success(f"✅ Exit Code: {result['returncode']}")
            else:
                st.error(f"❌ Exit Code: {result['returncode']}")
        with col_stats2:
            st.info(f"🔧 Local GCC")

# Estadísticas simplificadas
st.markdown("""
<div class="stats-container">
    <div class="stat-card">
        <div class="stat-number">Real</div>
        <div>Compiler Pipeline</div>
    </div>
    <div class="stat-card">
        <div class="stat-number">Local</div>
        <div>GCC Execution</div>
    </div>
    <div class="stat-card">
        <div class="stat-number">x86-64</div>
        <div>Assembly Target</div>
    </div>
    <div class="stat-card">
        <div class="stat-number">C99</div>
        <div>C Standard</div>
    </div>
</div>
""", unsafe_allow_html=True)

# Sidebar simplificado
with st.sidebar:
    st.markdown("### 🔧 Quick Actions")

    if st.button("🔄 Clear Assembly", use_container_width=True):
        keys_to_clear = ['assembly_output', 'assembly_execution_output']
        for key in keys_to_clear:
            if key in st.session_state:
                del st.session_state[key]
        st.rerun()

    if st.button("🗑️ Clear C Output", use_container_width=True):
        if 'c_execution_output' in st.session_state:
            del st.session_state['c_execution_output']
        st.rerun()

    if st.button("🆕 Clear All", use_container_width=True):
        keys_to_clear = ['assembly_output', 'assembly_execution_output', 'c_execution_output']
        for key in keys_to_clear:
            if key in st.session_state:
                del st.session_state[key]
        st.rerun()

    st.markdown("---")
    st.markdown("**📚 Setup:**")
    st.markdown("1. Compile: `g++ *.cpp -o compiler`")
    st.markdown("2. Install GCC for local assembly execution")
    st.markdown("3. Set JDoodle credentials for C execution")

    st.markdown("**🎯 Usage:**")
    st.markdown("• **Compile to Assembly**: Uses your real compiler")
    st.markdown("• **Run C Code**: Uses JDoodle API")
    st.markdown("• **Execute Assembly**: Uses local GCC")

    gcc_status = "✅ Available" if check_gcc_installation() else "❌ Not found"
    compiler_status = "✅ Found" if os.path.exists("./compiler") else "❌ Missing"

    st.markdown(f"**Status:**")
    st.markdown(f"• GCC: {gcc_status}")
    st.markdown(f"• Compiler: {compiler_status}")

# Footer simplificado
st.markdown("""
---
**🧪 Real Compiler Pipeline + Local GCC Assembly Execution**

- ✅ Your C to Assembly compiler integration  
- ✅ JDoodle C code execution (cloud)
- ✅ Local GCC assembly execution (fast & secure)
""")