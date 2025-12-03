# Lightweight base; good balance of size and features
FROM debian:bookworm-slim

# Prevent tz/locale prompts
ENV DEBIAN_FRONTEND=noninteractive

# Update and install LaTeX + tools
# - latexmk: auto-build tool
# - texlive-latex-recommended / -extra: core + minted
# - texlive-fonts-recommended: fonts you’ll likely use
# - python3-pygments: required by the minted package
# - ghostscript: useful for some packages (images/ps)
RUN apt-get update && apt-get install -y --no-install-recommends \
    make \
    latexmk \
    texlive-base \
    texlive-latex-recommended \
    texlive-latex-extra \
    texlive-fonts-recommended \
    texlive-fonts-extra \
    texlive-xetex \
    python3-pygments \
    ghostscript \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

# Work in /work and run as non-root for friendlier permissions
WORKDIR /work
RUN useradd -ms /bin/bash builder
USER builder

# Default command compiles main.tex with shell-escape for minted
# -file-line-error & -interaction=nonstopmode help readable logs in CI
ENTRYPOINT ["latexmk", "-pdf", "-shell-escape", "-file-line-error", "-interaction=nonstopmode"]
CMD ["main.tex"]
